#pragma once
#include "../libs/Exceptions.hpp"
#include <cstddef>
#include <initializer_list>

template<typename T>
class DynamicArray {
    T* data;
    std::size_t size;
public:
    using iterator = T*;
    using const_iterator = const T*;

    DynamicArray() : data(nullptr), size(0) {}
    DynamicArray(std::size_t n) : data(n ? new T[n] : nullptr), size(n) {}
    DynamicArray(T* items, std::size_t count) : data(count ? new T[count] : nullptr), size(count) {
        if (!items && count) throw InvalidArgument("DynamicArray: null items");
        for (std::size_t i = 0; i < count; ++i) data[i] = items[i];
    }
    DynamicArray(const DynamicArray& o) : data(o.size ? new T[o.size] : nullptr), size(o.size) {
        for (std::size_t i = 0; i < size; ++i) data[i] = o.data[i];
    }
    DynamicArray(std::initializer_list<T> l) : data(l.size() ? new T[l.size()] : nullptr), size(l.size()) {
        std::size_t i = 0;
        for (const T& v : l) data[i++] = v;
    }
    DynamicArray& operator=(const DynamicArray& o) {
        if (this == &o) return *this;
        T* nd = o.size ? new T[o.size] : nullptr;
        for (std::size_t i = 0; i < o.size; ++i) nd[i] = o.data[i];
        delete[] data;
        data = nd; size = o.size;
        return *this;
    }
    ~DynamicArray() { delete[] data; }

    T Get(std::size_t i) const {
        if (i >= size) throw IndexOutOfRange(i, size, "DynamicArray::Get");
        return data[i];
    }
    void Set(std::size_t i, T v) {
        if (i >= size) throw IndexOutOfRange(i, size, "DynamicArray::Set");
        data[i] = v;
    }
    std::size_t GetSize() const { return size; }
    T* GetRawData() const { return data; }
    void Resize(std::size_t n) {
        T* nd = n ? new T[n] : nullptr;
        std::size_t m = n < size ? n : size;
        for (std::size_t i = 0; i < m; ++i) nd[i] = data[i];
        delete[] data;
        data = nd; size = n;
    }
    iterator begin() { return data; }
    iterator end() { return data + size; }
    const_iterator begin() const { return data; }
    const_iterator end() const { return data + size; }
};