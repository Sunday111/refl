#pragma once

#include "EverydayTools/Bitset/EnumBitset.hpp"

namespace cppreflection
{
enum class TypeFlag
{
    None = 0,
    Common = (1 << 0),
    Number = (1 << 1),
    Enumeration = (1 << 3),
    Class = (1 << 4),
    CommonNumber = (1 << 0) | (1 << 1),
    CommonEnumeration = (1 << 0) | (1 << 3),
    CommonClass = (1 << 0) | (1 << 4),
    All = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4),
};
}  // namespace cppreflection

namespace edt
{
template <>
struct enable_enum_bitset<cppreflection::TypeFlag>
{
    static constexpr bool value = true;
};
}  // namespace edt
