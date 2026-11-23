#pragma once

template<typename A, typename B>
class Pair {
    A first_;
    B second_;
public:
    Pair() : first_(), second_() {}
    Pair(const A& a, const B& b) : first_(a), second_(b) {}
    A GetFirst() const { return first_; }
    B GetSecond() const { return second_; }
    bool operator==(const Pair& o) const { return first_ == o.first_ && second_ == o.second_; }
    bool operator<(const Pair& o) const {
        if (first_ != o.first_) return first_ < o.first_;
        return second_ < o.second_;
    }
};