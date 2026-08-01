#pragma once

#include "edt/bitset/enum_bitset.hpp"

namespace refl
{
enum class TypeFlag
{
    None = 0,
    Common = (1 << 0),
    Number = (1 << 1),
    Enumeration = (1 << 3),
    Class = (1 << 4),
};
}  // namespace refl

namespace edt
{
template <>
struct enable_enum_bitset<refl::TypeFlag>
{
    static constexpr bool value = true;
};
}  // namespace edt
