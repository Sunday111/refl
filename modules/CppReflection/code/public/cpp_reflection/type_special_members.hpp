#pragma once

namespace cppreflection
{
class TypeSpecialMembers
{
public:
    [[nodiscard]] constexpr bool operator==(const TypeSpecialMembers&) const = default;
    [[nodiscard]] constexpr bool operator!=(const TypeSpecialMembers&) const = default;

    using DefaultConstructor = void (*)(void* object);
    using CopyConstructor = void (*)(void* objectToConstruct, const void* referenceObject);
    using MoveConstructor = void (*)(void* destinationObject, void* sourceObject);
    using CopyAssign = void (*)(void* destination, const void* source);
    using MoveAssign = void (*)(void* destination, void* source);
    using Destructor = void (*)(void* object);

    DefaultConstructor defaultConstructor = nullptr;
    CopyConstructor copyConstructor = nullptr;
    MoveConstructor moveConstructor = nullptr;
    CopyAssign copyAssign = nullptr;
    MoveAssign moveAssign = nullptr;
    Destructor destructor = nullptr;
};
}  // namespace cppreflection
