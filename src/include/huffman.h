#pragma once
#include <memory>
#include <array>
#include <optional>
#include <queue>
#include <unordered_map>

namespace huffman
{

    struct ByteFrequency
    {
        ByteFrequency(uint32_t byte, std::size_t freq)
            : byte_code(byte), frequency(freq)
        {}

        ByteFrequency(std::size_t freq)
            : frequency(freq), byte_code(std::nullopt)
        {}

        ByteFrequency()
            : frequency(0), byte_code(std::nullopt)
        {}

        ByteFrequency(ByteFrequency &&bf)
            : byte_code(bf.byte_code), frequency(bf.frequency)
        {}

        ByteFrequency& operator=(ByteFrequency&& other) noexcept {
            if (this != &other)
            {
                byte_code = other.byte_code;
                frequency = other.frequency;
            }
            return *this;
        }

        std::optional<uint8_t>          byte_code;
        std::size_t                     frequency;
    };

    struct Node
    {
        Node()
            : info(), l(nullptr), r(nullptr)
        {}
        Node(
            ByteFrequency&& bfreq
        ) noexcept
            : l(nullptr), r(nullptr), info(std::move(bfreq))
        {}

        Node(Node &&node) noexcept
            : info(std::move(node.info)), l(std::move(node.l)), r(std::move(node.r))
        {}

        Node &operator=(Node &&node) noexcept
        {
            if (this != &node)
            {
                l = std::move(node.l);
                r = std::move(node.r);
                info = std::move(node.info);
            }

            return *this;
        }

        ~Node() = default;

        ByteFrequency           info;
        std::unique_ptr<Node>   l;
        std::unique_ptr<Node>   r;
    };

    struct NodeCompare
    {
        bool operator()(const std::unique_ptr<Node>& l, const std::unique_ptr<Node>& r)
        {
            return l->info.frequency > r->info.frequency;
        }
    };

    struct HuffmanTree
    {
        HuffmanTree()
            : root(nullptr)
        {}

        std::unique_ptr<Node> root;

        void BuildTree(std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, NodeCompare> &&pque) noexcept
        {
            while (pque.size() > 1)
            {
                auto left = std::move(const_cast<std::unique_ptr<Node>&>(pque.top()));
                pque.pop();

                auto right = std::move(const_cast<std::unique_ptr<Node>&>(pque.top()));
                pque.pop();

                auto frequency = left->info.frequency + right->info.frequency;
                std::unique_ptr<Node> parent = std::make_unique<Node>(ByteFrequency(frequency));

                parent->l = std::move(left);
                parent->r = std::move(right);

                pque.push(std::move(parent));
            }

            root = std::move(const_cast<std::unique_ptr<Node>&>(pque.top()));
            pque.pop();
        }

    };

    void CollectCodes(Node *node, std::vector<bool> value, std::unordered_map<uint32_t, std::vector<bool>> &result)
    {
        if (node->l == nullptr && node->r == nullptr)
        {
            if (value.size() == 0)
            {
                value.push_back(false);
            }
            result[*node->info.byte_code] = value;
            return;
        }

        value.push_back(false);
        CollectCodes(node->l.get(), value, result);
        value.pop_back();

        value.push_back(true);
        CollectCodes(node->r.get(), value, result);
        value.pop_back();
    }

    std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, NodeCompare> transform(
        std::array<size_t, 256> &&arr
    )
    {
        std::priority_queue<
            std::unique_ptr<Node>,
            std::vector<std::unique_ptr<Node>>,
            NodeCompare> q;

        for (uint32_t code = 0; code < 256; code++)
        {
            if (arr[code] != 0)
            {
                q.emplace(std::make_unique<Node>(ByteFrequency{code, arr[code]}));
            }
        }

        return q;
    }


}