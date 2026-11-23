#pragma once
#include "IDictionary.hpp"
#include "LinkedList.hpp"
#include "../libs/Pair.hpp"
#include "../libs/Exceptions.hpp"
#include <cstdint>
#include <string>
#include <cstddef>

template<typename T> struct KeyHash;

template<> struct KeyHash<int> {
    std::size_t operator()(int k) const {
        return static_cast<std::size_t>(k) * 11400714819323198485ULL;
    }
};

template<> struct KeyHash<std::string> {
    std::size_t operator()(const std::string& s) const {
        std::size_t h = 14695981039346656037ULL;
        for (char c : s) { h ^= static_cast<unsigned char>(c); h *= 1099511628211ULL; }
        return h;
    }
};

template<typename A, typename B>
struct KeyHash<Pair<A, B>> {
    std::size_t operator()(const Pair<A, B>& p) const {
        std::size_t h1 = KeyHash<A>{}(p.GetFirst());
        std::size_t h2 = KeyHash<B>{}(p.GetSecond());
        return h1 ^ (h2 * 11400714819323198485ULL + (h1 << 6) + (h1 >> 2));
    }
};

template<typename K, typename V>
class HashTableDictionary : public IDictionary<K, V> {
    struct Entry {
        K key;
        V value;
        Entry() : key(), value() {}
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

    MutableArraySequence<LinkedList<Entry>*> buckets;
    unsigned int count;
    std::size_t capacity;

    std::size_t hash(const K& key) const { return KeyHash<K>{}(key) % capacity; }

public:
    explicit HashTableDictionary(std::size_t cap = 1024)
        : buckets(cap), count(0), capacity(cap)
    {
        for (std::size_t i = 0; i < cap; ++i) buckets.Set(i, new LinkedList<Entry>());
    }
    ~HashTableDictionary() {
        for (std::size_t i = 0; i < buckets.GetLength(); ++i) delete buckets.Get(i);
    }
    HashTableDictionary(const HashTableDictionary&) = delete;
    HashTableDictionary& operator=(const HashTableDictionary&) = delete;

    bool contains_key(const K& key) const override {
        LinkedList<Entry>* b = buckets.Get(hash(key));
        for (std::size_t i = 0; i < b->GetLength(); ++i)
            if (b->Get(i).key == key) return true;
        return false;
    }
    void add(const K& key, const V& value) override {
        LinkedList<Entry>* b = buckets.Get(hash(key));
        for (std::size_t i = 0; i < b->GetLength(); ++i)
            if (b->Get(i).key == key) { b->Set(i, Entry(key, value)); return; }
        b->Append(Entry(key, value));
        ++count;
    }
    void remove_by_key(const K& key) override {
        std::size_t idx = hash(key);
        LinkedList<Entry>* b = buckets.Get(idx);
        LinkedList<Entry>* nb = new LinkedList<Entry>();
        bool found = false;
        for (std::size_t i = 0; i < b->GetLength(); ++i) {
            Entry e = b->Get(i);
            if (e.key == key) { found = true; continue; }
            nb->Append(e);
        }
        if (!found) { delete nb; throw InvalidArgument("remove_by_key: key not found"); }
        delete b;
        buckets.Set(idx, nb);
        --count;
    }
    V get_elem_by_key(const K& key) const override {
        LinkedList<Entry>* b = buckets.Get(hash(key));
        for (std::size_t i = 0; i < b->GetLength(); ++i)
            if (b->Get(i).key == key) return b->Get(i).value;
        throw InvalidArgument("get_elem_by_key: key not found");
    }
    V& get_ref_by_key(const K& key) override {
        LinkedList<Entry>* b = buckets.Get(hash(key));
        for (std::size_t i = 0; i < b->GetLength(); ++i)
            if (b->Get(i).key == key) return b->GetRef(i).value;
        throw InvalidArgument("get_ref_by_key: key not found");
    }
    MutableArraySequence<K> get_keys() const override {
        MutableArraySequence<K> out;
        for (std::size_t i = 0; i < buckets.GetLength(); ++i) {
            LinkedList<Entry>* b = buckets.Get(i);
            for (std::size_t j = 0; j < b->GetLength(); ++j) out.Append(b->Get(j).key);
        }
        return out;
    }
    unsigned int get_count() const override { return count; }
};