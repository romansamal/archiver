#pragma once

#include <cstdint>
#include "interfaces.h"
#include <unordered_map>
#include <thread>
#include <numeric>
#include <vector>

class HuffmanAlgorithm: public CompressAlgorithm<uint8_t>
{
public:
    HuffmanAlgorithm() = default;
    ~HuffmanAlgorithm() = default;

    std::size_t Compress(const RandomAccessible<uint8_t> &object_from, RandomAccessible<uint8_t> &object_to) override;

    std::size_t Uncompress(const RandomAccessible<uint8_t> &object_from, RandomAccessible<uint8_t> &object_to) override;

private:

    std::array<size_t, 256> GetFrequencies(const RandomAccessible<uint8_t> &from);

    std::size_t GetCompressedSize(
        const RandomAccessible<uint8_t> &object_from,
        const std::unordered_map<uint32_t, std::vector<bool>>& huffmanCodes
    );

    void Encode(
        const RandomAccessible<uint8_t> &object_from,
        RandomAccessible<uint8_t>       &object_to,
        const std::unordered_map<uint32_t, std::vector<bool>>& huffmanCodes);
};