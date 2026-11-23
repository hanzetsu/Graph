#pragma once
#include <string>
#include <sstream>
#include <cstddef>

class IException {
public:
    virtual ~IException() = default;
    virtual const char* what() const noexcept = 0;
};

class DataStructureException : public IException {
    std::string message;
public:
    explicit DataStructureException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class IndexOutOfRange : public DataStructureException {
public:
    IndexOutOfRange(std::size_t index, std::size_t size, const std::string& context)
        : DataStructureException(build(index, size, context)) {}
private:
    static std::string build(std::size_t index, std::size_t size, const std::string& ctx) {
        std::ostringstream oss;
        oss << ctx << ": index " << index << " out of range [0, "
            << (size == 0 ? 0 : size - 1) << "]";
        return oss.str();
    }
};

class EmptyStructureException : public DataStructureException {
public:
    explicit EmptyStructureException(const std::string& ctx)
        : DataStructureException(ctx + ": empty") {}
};

class InvalidArgument : public DataStructureException {
public:
    explicit InvalidArgument(const std::string& msg) : DataStructureException(msg) {}
};