#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "CppReflection/CallReflectedFunction.hpp"
#include "CppReflection/Reflector/TypeReflector.hpp"
#include "gtest/gtest.h"

TEST(CppReflectionTest, SpecialMembers_DefaultConstructor)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_special_members::default_constructor::ReflectedType");
        }

        ReflectedType() : member(124) {}

        int member = 112;
    };

    const cppreflection::Type* type = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().defaultConstructor != nullptr);

    alignas(ReflectedType) std::array<std::byte, sizeof(ReflectedType)> data{};
    auto* pointer = reinterpret_cast<ReflectedType*>(data.data());
    type->GetSpecialMembers().defaultConstructor(pointer);
    pointer = std::launder(pointer);
    EXPECT_TRUE(pointer->member == 124);
    type->GetSpecialMembers().destructor(pointer);
}

TEST(CppReflectionTest, SpecialMembers_NoDefaultConstructor)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_special_members::no_default_constructor::ReflectedType");
        }

        explicit ReflectedType(int) {}
    };

    const cppreflection::Type* type = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().defaultConstructor == nullptr);
}

TEST(CppReflectionTest, SpecialMembers_CopyAssign)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_special_members::copy_assign::ReflectedType");
        }

        ReflectedType() = default;
        ReflectedType(const ReflectedType&) = default;
        ReflectedType& operator=(const ReflectedType&) = default;

        std::vector<int> member;
    };

    const cppreflection::Type* type = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().copyAssign != nullptr);

    ReflectedType a, b;
    std::vector<int> member{1, 2, 3};
    b.member = member;
    type->GetSpecialMembers().copyAssign(&a, &b);
    EXPECT_TRUE(b.member == member);
    EXPECT_TRUE(a.member == member);
}

TEST(CppReflectionTest, SpecialMembers_NoCopyAssign)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_special_members::no_copy_assign::ReflectedType");
        }

        ReflectedType(const ReflectedType& another) = delete;
        ReflectedType& operator=(const ReflectedType& another) = delete;
    };

    const cppreflection::Type* type = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().copyAssign == nullptr);
}

TEST(CppReflectionTest, SpecialMembers_MoveAssign)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_special_members::move_assign::ReflectedType");
        }

        ReflectedType& operator=(ReflectedType&&) noexcept = default;

        std::vector<int> member;
    };

    const cppreflection::Type* type = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().moveAssign != nullptr);

    ReflectedType a, b;
    std::vector<int> member{1, 2, 3};
    b.member = member;
    type->GetSpecialMembers().moveAssign(&a, &b);
    EXPECT_TRUE(b.member.empty());
    EXPECT_TRUE(a.member == member);
}

TEST(CppReflectionTest, SpecialMembers_NoMoveAssign)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_special_members::no_move_assign::ReflectedType");
        }

        ReflectedType& operator=(ReflectedType&& another) = delete;
    };

    const cppreflection::Type* type = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().moveAssign == nullptr);
}

TEST(CppReflectionTest, SpecialMembers_Destructor)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_special_members::destructor::ReflectedType");
        }

        ~ReflectedType()
        {
            if (destroyed != nullptr)
            {
                *destroyed = true;
            }
        }

        bool* destroyed = nullptr;
    };

    const cppreflection::Type* type = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().defaultConstructor != nullptr);
    EXPECT_TRUE(type->GetSpecialMembers().destructor != nullptr);

    alignas(ReflectedType) std::array<std::byte, sizeof(ReflectedType)> data{};
    auto* pointer = reinterpret_cast<ReflectedType*>(data.data());
    type->GetSpecialMembers().defaultConstructor(pointer);
    pointer = std::launder(pointer);
    bool destroyed = false;
    pointer->destroyed = &destroyed;
    type->GetSpecialMembers().destructor(pointer);
    EXPECT_TRUE(destroyed);
}
