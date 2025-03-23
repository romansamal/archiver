#pragma once 
#include <stdexcept>
#include <cstdint>

template <typename T>
class RandomAccessible
{
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;

    virtual ~RandomAccessible() = default;

    virtual size_type size() const noexcept = 0;

    virtual bool emtpy() const noexcept
    {
         return (size() == 0);
    }

    virtual reference operator[](size_type position) = 0;

    virtual const_reference operator[](size_type position) const = 0;

    virtual reference at(size_type position)
    {
        if (position > size())
        {
            throw std::out_of_range("Wrong Read Object position");
        }

        return (*this)[position];
    }

    virtual const_reference at(size_type position) const
    {
        if (position > size())
        {
            throw std::out_of_range("Wrong Read Object position");
        }

        return (*this)[position];
    }

    class iterator;
    class const_iterator;

    virtual iterator begin() noexcept = 0;
    virtual iterator end() noexcept = 0;
    virtual const_iterator begin() const noexcept = 0;
    virtual const_iterator end() const noexcept = 0;
    virtual const_iterator cbegin() const noexcept = 0;
    virtual const_iterator cend() const noexcept = 0;

    class iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(pointer ptr = nullptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() { return ptr_; }

        iterator& operator++() { ++ptr_; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        iterator& operator--() { --ptr_; return *this; }
        iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

        iterator operator+(difference_type n) const { return iterator(ptr_ + n); }
        iterator operator-(difference_type n) const { return iterator(ptr_ - n); }

        difference_type operator-(const iterator& other) const { return ptr_ - other.ptr_; }

        bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const iterator& other) const { return ptr_ < other.ptr_; }

        reference operator[](difference_type n) const { return ptr_[n]; }

    private:
        pointer ptr_;
    };

    class const_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const uint8_t;
        using difference_type = std::ptrdiff_t;
        using pointer = const uint8_t*;
        using reference = const uint8_t&;

        const_iterator(pointer ptr = nullptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() { return ptr_; }

        const_iterator& operator++() { ++ptr_; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

        const_iterator& operator--() { --ptr_; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }

        const_iterator operator+(difference_type n) const { return const_iterator(ptr_ + n); }
        const_iterator operator-(difference_type n) const { return const_iterator(ptr_ - n); }

        difference_type operator-(const const_iterator& other) const { return ptr_ - other.ptr_; }

        bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const const_iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const const_iterator& other) const { return ptr_ < other.ptr_; }

        reference operator[](difference_type n) const { return ptr_[n]; }

    private:
        pointer ptr_;
    };
};

template <typename T>
class CompressAlgorithm
{
public:
    ~CompressAlgorithm() = default;

    /**
     * Compress specified radnom accessible object
     * + size of compressed file
     */
    virtual std::size_t Compress(const RandomAccessible<T> &object_from, RandomAccessible<T> &object_to) = 0;

    /**
     * Uncompress specified radnom accessible objcet
     * + size of uncompressed objcect
     */
    virtual std::size_t Uncompress(const RandomAccessible<T> &object_from, RandomAccessible<T> &object_to) = 0;
};
