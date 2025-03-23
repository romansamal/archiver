#pragma once 

#include "fileutils.hpp"
#include "interfaces.h"


class FileRandomAccessible : public fileutils::File, public RandomAccessible<uint8_t>
{
public:
FileRandomAccessible(
        const std::string_view file_path)
        : File(file_path)
    {
        Open();

        cachedSize = File::GetSize();
    }

    ~FileRandomAccessible()
    {
        Close();
    }

    size_type size() const noexcept
    {
        return cachedSize;
    }

    uint8_t &operator[](size_t position) override
    {
        return (reinterpret_cast<uint8_t *>(sink.data())[position]);
    }

    const uint8_t &operator[](size_t position) const override
    {
        return (reinterpret_cast<uint8_t *>(sink.data())[position]);
    }

    iterator begin() noexcept override
    {
        return iterator(reinterpret_cast<uint8_t*>(sink.data()));
    }

    iterator end() noexcept override
    {
        return iterator(reinterpret_cast<uint8_t*>(sink.data()) + size());
    }

    const_iterator begin() const noexcept override
    {
        return const_iterator(reinterpret_cast<const uint8_t*>(sink.data()));
    }

    const_iterator end() const noexcept override
    {
        return const_iterator(reinterpret_cast<const uint8_t*>(sink.data()) + size());
    }

    const_iterator cbegin() const noexcept override
    {
        return begin();
    }

    const_iterator cend() const noexcept override
    {
        return end();
    }

private:
    size_type cachedSize;
};
