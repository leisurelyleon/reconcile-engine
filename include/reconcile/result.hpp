#ifndef RECONCILE_RESULT_HPP
#define RECONCILE_RESULT_HPP

#include <string>
#include <utility>
#include <variant>

namespace reconcile {

/// An error carrier used by Result.
struct Error {
    std::string message;
};

/// A lightweight success-or-error type for handling failures explicitly,
/// without exceptions in the reconciliation path.
template <typename T>
class Result {
public:
    [[nodiscard]] static Result ok(T item) { return Result(std::move(item)); }

    [[nodiscard]] static Result err(std::string message) {
        return Result(Error{std::move(message)});
    }

    [[nodiscard]] bool is_ok() const { return std::holds_alternative<T>(data_); }
    [[nodiscard]] bool is_err() const { return std::holds_alternative<Error>(data_); }

    [[nodiscard]] const T& value() const { return std::get<T>(data_); }
    [[nodiscard]] const std::string& error() const { return std::get<Error>(data_).message; }

private:
    explicit Result(T success) : data_(std::move(success)) {}
    explicit Result(Error failure) : data_(std::move(failure)) {}

    std::variant<T, Error> data_;
};

}  // namespace reconcile

#endif  // RECONCILE_RESULT_HPP
