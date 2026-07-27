#pragma once

#include <concepts>
#include <functional>

namespace refl::detail
{
template <typename T>
concept EqualityComparable = requires(const T& lhs, const T& rhs) {
    { std::equal_to<>{}(lhs, rhs) } -> std::convertible_to<bool>;
};

template <typename T>
concept LessThanComparable = requires(const T& lhs, const T& rhs) {
    { std::less<>{}(lhs, rhs) } -> std::convertible_to<bool>;
};

template <typename T>
inline constexpr bool has_equals_operator_v = EqualityComparable<T>;

template <typename T>
inline constexpr bool has_less_operator_v = LessThanComparable<T>;
}  // namespace refl::detail
