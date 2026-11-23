#pragma once
#include "MutableArraySequence.hpp"

template<typename K, typename V>
class IDictionary {
public:
    virtual ~IDictionary() = default;
    virtual bool contains_key(const K& key) const = 0;
    virtual void add(const K& key, const V& value) = 0;
    virtual void remove_by_key(const K& key) = 0;
    virtual V get_elem_by_key(const K& key) const = 0;
    virtual V& get_ref_by_key(const K& key) = 0;
    virtual MutableArraySequence<K> get_keys() const = 0;
    virtual unsigned int get_count() const = 0;
};