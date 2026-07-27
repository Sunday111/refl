#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "../type_special_members.hpp"

namespace cppreflection::detail
{
template <typename T>
[[nodiscard]]
constexpr TypeSpecialMembers::DefaultConstructor MakeDefaultConstructor() noexcept
{
    if constexpr (!std::is_default_constructible_v<T>)
    {
        return nullptr;
    }
    else
    {
        return [](void* object)
        {
            std::construct_at(static_cast<T*>(object));
        };
    }
}

template <typename T>
[[nodiscard]]
constexpr TypeSpecialMembers::CopyConstructor MakeCopyConstructor() noexcept
{
    if constexpr (!std::is_copy_constructible_v<T>)
    {
        return nullptr;
    }
    else
    {
        return [](void* objectToConstruct, const void* referenceObject)
        {
            std::construct_at(static_cast<T*>(objectToConstruct), *static_cast<const T*>(referenceObject));
        };
    }
}

template <typename T>
[[nodiscard]]
constexpr TypeSpecialMembers::MoveConstructor MakeMoveConstructor() noexcept
{
    if constexpr (!std::is_move_constructible_v<T>)
    {
        return nullptr;
    }
    else
    {
        return [](void* destinationObject, void* sourceObject)
        {
            std::construct_at(static_cast<T*>(destinationObject), std::move(*static_cast<T*>(sourceObject)));
        };
    }
}

template <typename T>
[[nodiscard]]
constexpr TypeSpecialMembers::CopyAssign MakeCopyAssignOperator() noexcept
{
    if constexpr (!std::is_copy_assignable_v<T>)
    {
        return nullptr;
    }
    else
    {
        return [](void* destination, const void* source)
        {
            const T& src = *static_cast<const T*>(source);
            T& dst = *static_cast<T*>(destination);
            dst = src;
        };
    }
}

template <typename T>
[[nodiscard]]
constexpr TypeSpecialMembers::MoveAssign MakeMoveAssignOperator() noexcept
{
    if constexpr (!std::is_move_assignable_v<T>)
    {
        return nullptr;
    }
    else
    {
        return [](void* destination, void* source)
        {
            T& src = *static_cast<T*>(source);
            T& dst = *static_cast<T*>(destination);
            dst = std::move(src);
        };
    }
}

template <typename T>
[[nodiscard]]
constexpr TypeSpecialMembers::Destructor MakeDestructor() noexcept
{
    if constexpr (!std::is_destructible_v<T>)
    {
        return nullptr;
    }
    else
    {
        return [](void* object)
        {
            std::destroy_at(static_cast<T*>(object));
        };
    }
}

template <typename T>
[[nodiscard]]
constexpr TypeSpecialMembers MakeTypeSpecialMembers()
{
    TypeSpecialMembers result{};

    result.defaultConstructor = MakeDefaultConstructor<T>();
    result.copyConstructor = MakeCopyConstructor<T>();
    result.moveConstructor = MakeMoveConstructor<T>();
    result.copyAssign = MakeCopyAssignOperator<T>();
    result.moveAssign = MakeMoveAssignOperator<T>();
    result.destructor = MakeDestructor<T>();

    return result;
}
}  // namespace cppreflection::detail
