#pragma once
#include <stdexcept>
#include <string>
#include <sstream>
#include <source_location>

class graph_exception : public std::runtime_error {
public:
    graph_exception(const std::string& msg, const std::source_location& loc = std::source_location::current())
        : std::runtime_error(fmt(msg, loc)) {}
private:
    static std::string fmt(const std::string& msg, const std::source_location& loc) {
        std::ostringstream os;
        os << "[" << loc.file_name() << ":" << loc.line() << "] " << msg;
        return os.str();
    }
};

class vertex_not_found : public graph_exception {
public:
    using graph_exception::graph_exception;
};