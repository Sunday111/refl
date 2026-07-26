#pragma once

#include <functional>

#include "../TypeComparisonOperators.hpp"
#include "HasOperator.hpp"

namespace cppreflection::detail
{
template <typename T>
[[nodiscard]] constexpr TypeComparisonOperators MakeTypeComparisonOperators()
{
    TypeComparisonOperators result{};

    if constexpr (has_equals_operator_v<T>)
    {
        result.equals = [](const void* a, const void* b)
        {
            const auto* ca = static_cast<const T*>(a);
            const auto* cb = static_cast<const T*>(b);
            return std::equal_to<>{}(*ca, *cb);
        };
    }

    if constexpr (has_less_operator_v<T>)
    {
        result.less = [](const void* a, const void* b)
        {
            const auto* ca = static_cast<const T*>(a);
            const auto* cb = static_cast<const T*>(b);
            return std::less<>{}(*ca, *cb);
        };
    }

    return result;
}
}  // namespace cppreflection::detail
