#pragma once
#include <stdexcept>
#include <string>

class graph_exception : public std::runtime_error {
public:
    explicit graph_exception(const std::string& msg) : std::runtime_error(msg) {}
};

class vertex_not_found : public graph_exception {
public:
    using graph_exception::graph_exception;
};