#include "huffman_algorithm.h"

#include "huffman.h"

std::size_t HuffmanAlgorithm::Compress(const RandomAccessible<uint8_t> &object_from, RandomAccessible<uint8_t> &object_to)
{
    huffman::HuffmanTree tree;

    tree.BuildTree(
        huffman::transform(GetFrequencies(object_from))
    );

    std::unordered_map<uint32_t, std::vector<bool>> huffmanCodes{};
    std::vector<bool> codes{};
    CollectCodes(tree.root.get(), codes, huffmanCodes);

    std::size_t out_size = GetCompressedSize(object_from, huffmanCodes);

    Encode(object_from, object_to, huffmanCodes);

    return out_size;
}

std::size_t HuffmanAlgorithm::Uncompress(const RandomAccessible<uint8_t> &object_from, RandomAccessible<uint8_t> &object_to)
{

    return 0;
}

std::array<size_t, 256> HuffmanAlgorithm::GetFrequencies(const RandomAccessible<uint8_t> &from)
{
    constexpr size_t cache_line_size = 64;
    constexpr size_t chunk_size = 4 * 1024 * 1024; // 4MB chunks
    const unsigned num_threads = std::thread::hardware_concurrency();
    std::vector<std::array<size_t, 256>> thread_counts(num_threads);
    std::vector<std::thread> workers;

    auto worker = [&](size_t start, size_t end, std::array<size_t, 256>& counts) {
            // Align start to cache line
            start = (start) & ~(cache_line_size - 1);

            // Process in chunks
            for (size_t i = start; i < end; i += cache_line_size) {
                size_t chunk_end = std::min(i + cache_line_size, end);
                for (size_t j = i; j < chunk_end; ++j) {
                    ++counts[static_cast<uint8_t>(from[j])];
                }
            }
    };
    // Divide work among threads
    size_t chunk = from.size() / num_threads;
    for (unsigned i = 0; i < num_threads; ++i) {
        size_t start = i * chunk;
        size_t end = (i == num_threads - 1) ? from.size() : start + chunk;
        workers.emplace_back(worker, start, end, std::ref(thread_counts[i]));
    }

    // Wait for all threads
    for (auto& t : workers) {
        t.join();
    }

    // Combine results
    std::array<size_t, 256> frequencies{};
    for (const auto& counts : thread_counts) {
        for (size_t i = 0; i < 256; ++i) {
            frequencies[i] += counts[i];
        }
    }

    return frequencies;
}

std::size_t HuffmanAlgorithm::GetCompressedSize(
    const RandomAccessible<uint8_t> &object_from,
    const std::unordered_map<uint32_t, std::vector<bool>>& huffmanCodes
)
{
    // Precompute code table and code lengths
    std::array<uint8_t, 256> codeLengths{};
    for (const auto& [byte, code] : huffmanCodes) {
        if (byte <= 255) {
            codeLengths[byte] = code.size();
        }
    }

    // Calculate output size in parallel
    const size_t dataSize = object_from.size();
    const unsigned threadCount = std::thread::hardware_concurrency();
    std::vector<size_t> partialCounts(threadCount);
    std::vector<std::thread> workers;

    auto countWorker = [&](size_t start, size_t end, size_t& result) {
        size_t count = 0;
        for (size_t i = start; i < end; ++i) {
            count += codeLengths[static_cast<uint8_t>(object_from[i])];
        }
        result = count;
    };

    const size_t chunkSize = dataSize / threadCount;
    for (unsigned i = 0; i < threadCount; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == threadCount - 1) ? dataSize : start + chunkSize;
        workers.emplace_back(countWorker, start, end, std::ref(partialCounts[i]));
    }

    for (auto& t : workers) {
        t.join();
    }

    const size_t bitCount = std::accumulate(partialCounts.begin(), partialCounts.end(), 0UL);
    const size_t outputSize = (bitCount + 7) / 8;

    return outputSize;
}

void HuffmanAlgorithm::Encode(
    const RandomAccessible<uint8_t> &object_from,
    RandomAccessible<uint8_t>       &object_to,
    const std::unordered_map<uint32_t, std::vector<bool>>& huffmanCodes)
{
    // Precompute code table and code lengths
    std::array<std::vector<bool>, 256> codeTable{};
    for (const auto& [byte, code] : huffmanCodes) {
        if (byte <= 255) {
            codeTable[byte] = code;
        }
    }
    const unsigned threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;
    const size_t chunkSize = object_from.size() / threadCount;

    // Parallel encoding with thread-local buffers
    std::vector<std::vector<uint8_t>> threadBuffers(threadCount);
    std::vector<size_t> threadBitPositions(threadCount, 7);
    std::vector<size_t> threadByteCounts(threadCount, 0);

    auto encodeWorker = [&](size_t start, size_t end, size_t threadIdx) {
        uint8_t currentByte = 0;
        uint8_t bitPos = 7;
        std::vector<uint8_t> buffer;
        buffer.reserve((end - start) / 2); // Estimate buffer size

        for (size_t i = start; i < end; ++i) {
            uint8_t byte = static_cast<uint8_t>(object_from[i]);
            const auto& code = codeTable[byte];

            for (bool bit : code) {
                if (bit) {
                    currentByte |= (1 << bitPos);
                }

                if (--bitPos < 0) {
                    buffer.push_back(currentByte);
                    currentByte = 0;
                    bitPos = 7;
                }
            }
        }

        // Store remaining bits
        if (bitPos != 7) {
            buffer.push_back(currentByte);
        }

        threadBuffers[threadIdx] = std::move(buffer);
        threadBitPositions[threadIdx] = bitPos;
        threadByteCounts[threadIdx] = buffer.size();
    };

    for (unsigned i = 0; i < threadCount; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == threadCount - 1) ? object_from.size() : start + chunkSize;
        workers.emplace_back(encodeWorker, start, end, i);
    }

    for (auto& t : workers) {
        t.join();
    }

    // Merge thread buffers
    size_t outputPos = 0;
    for (size_t i = 0; i < threadCount; ++i) {
        const auto& buffer = threadBuffers[i];
        size_t buffer_pos = 0;
        for (std::size_t pos = outputPos; pos < outputPos + buffer.size(); pos++)
        {
            object_to[pos] = buffer[buffer_pos++];
        }
        outputPos += buffer.size();
    }
}