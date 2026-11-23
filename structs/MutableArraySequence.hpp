#pragma once
#include "DynamicArray.hpp"
#include "Sequence.hpp"
#include "../libs/Exceptions.hpp"

template<typename T>
class MutableArraySequence : public Sequence<T> {
    DynamicArray<T> data;
public:
    MutableArraySequence() : data(0) {}
    MutableArraySequence(std::size_t n) : data(n) {}
    MutableArraySequence(T* items, std::size_t count) : data(items, count) {}
    MutableArraySequence(std::initializer_list<T> l) : data(l) {}

    T GetFirst() const override {
        if (GetLength() == 0) throw EmptyStructureException("MutableArraySequence::GetFirst");
        return data.Get(0);
    }
    T GetLast() const override {
        std::size_t n = GetLength();
        if (n == 0) throw EmptyStructureException("MutableArraySequence::GetLast");
        return data.Get(n - 1);
    }
    T Get(std::size_t i) const override { return data.Get(i); }
    std::size_t GetLength() const override { return data.GetSize(); }

    Sequence<T>* GetSubsequence(std::size_t start, std::size_t end) const override {
        std::size_t n = GetLength();
        if (start > end || end >= n) throw IndexOutOfRange(end, n, "GetSubsequence");
        MutableArraySequence<T>* sub = new MutableArraySequence<T>(end - start + 1);
        for (std::size_t i = 0; i < end - start + 1; ++i) sub->data.Set(i, data.Get(start + i));
        return sub;
    }
    Sequence<T>* Append(T item) override {
        std::size_t n = GetLength();
        data.Resize(n + 1);
        data.Set(n, item);
        return this;
    }
    Sequence<T>* Prepend(T item) override {
        std::size_t n = GetLength();
        DynamicArray<T> nd(n + 1);
        nd.Set(0, item);
        for (std::size_t i = 0; i < n; ++i) nd.Set(i + 1, data.Get(i));
        data = nd;
        return this;
    }
    Sequence<T>* InsertAt(T item, std::size_t index) override {
        std::size_t n = GetLength();
        if (index > n) throw IndexOutOfRange(index, n + 1, "InsertAt");
        DynamicArray<T> nd(n + 1);
        for (std::size_t i = 0; i < index; ++i) nd.Set(i, data.Get(i));
        nd.Set(index, item);
        for (std::size_t i = index; i < n; ++i) nd.Set(i + 1, data.Get(i));
        data = nd;
        return this;
    }
    Sequence<T>* Concat(Sequence<T>* s) const override {
        std::size_t n1 = GetLength(), n2 = s->GetLength();
        MutableArraySequence<T>* r = new MutableArraySequence<T>(n1 + n2);
        for (std::size_t i = 0; i < n1; ++i) r->data.Set(i, data.Get(i));
        for (std::size_t i = 0; i < n2; ++i) r->data.Set(n1 + i, s->Get(i));
        return r;
    }

    void Set(std::size_t i, T v) { data.Set(i, v); }
    T& operator[](std::size_t i) {
        if (i >= GetLength()) throw IndexOutOfRange(i, GetLength(), "operator[]");
        return data.GetRawData()[i];
    }
    T operator[](std::size_t i) const { return data.Get(i); }
    void RemoveAt(std::size_t i) {
        std::size_t n = GetLength();
        if (i >= n) throw IndexOutOfRange(i, n, "RemoveAt");
        DynamicArray<T> nd(n - 1);
        std::size_t j = 0;
        for (std::size_t k = 0; k < n; ++k) if (k != i) nd.Set(j++, data.Get(k));
        data = nd;
    }
    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
};