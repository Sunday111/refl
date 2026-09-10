#pragma once

#include <type_traits>

namespace refl::detail
{
template <typename T>
struct MemberPointerOwner;

template <typename Member, typename Class>
struct MemberPointerOwner<Member Class::*>
{
    using Type = Class;
};

template <auto pointer, typename Reflected>
concept DeclaredByReflectedType =
    !std::is_member_pointer_v<decltype(pointer)> ||
    std::is_same_v<typename MemberPointerOwner<decltype(pointer)>::Type, std::remove_cvref_t<Reflected>>;
}  // namespace refl::detail
