#pragma once
#include <stdexcept>

template<typename T>
class Option {
    bool hasValue;
    T value;
public:
    Option() : hasValue(false), value() {}
    Option(const T& val) : hasValue(true), value(val) {}
    bool HasValue() const { return hasValue; }
    T GetValue() const {
        if (!hasValue) throw std::logic_error("Option empty");
        return value;
    }
    T GetValueOrDefault(const T& def) const { return hasValue ? value : def; }
};