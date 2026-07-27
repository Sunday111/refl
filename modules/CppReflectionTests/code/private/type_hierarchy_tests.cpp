#include "cpp_reflection/get_static_type_info.hpp"
#include "cpp_reflection/get_type_info.hpp"
#include "gtest/gtest.h"

namespace
{
struct BaseType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return cppreflection::StaticClassTypeInfo<BaseType>(
            "BaseType",
            edt::GUID::Create("57630F4D-54C8-43CA-A0F8-540967C4C06B"));
    }
};

struct MiddleType : BaseType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return cppreflection::StaticClassTypeInfo<MiddleType>(
                   "MiddleType",
                   edt::GUID::Create("0E44E448-0BBF-4501-8706-5957068485E0"))
            .Base<BaseType>();
    }
};

struct DerivedType : MiddleType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return cppreflection::StaticClassTypeInfo<DerivedType>(
                   "DerivedType",
                   edt::GUID::Create("91FC8A42-E128-4465-81B0-43ED67D3B7F5"))
            .Base<MiddleType>();
    }
};

struct UnrelatedType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return cppreflection::StaticClassTypeInfo<UnrelatedType>(
            "UnrelatedType",
            edt::GUID::Create("5E057529-328C-4245-8DE1-E17146CB9EB4"));
    }
};
}  // namespace

TEST(TypeHierarchyTest, TraversesRegisteredBaseTypes)
{
    const auto* derived = cppreflection::GetTypeInfo<DerivedType>();
    const auto* middle = cppreflection::GetTypeInfo<MiddleType>();
    const auto* base = cppreflection::GetTypeInfo<BaseType>();
    const auto* unrelated = cppreflection::GetTypeInfo<UnrelatedType>();
    EXPECT_TRUE(derived->IsA(derived->GetGuid()));
    EXPECT_TRUE(derived->IsA(middle->GetGuid()));
    EXPECT_TRUE(derived->IsA(base->GetGuid()));
    EXPECT_FALSE(base->IsA(derived->GetGuid()));
    EXPECT_FALSE(derived->IsA(unrelated->GetGuid()));
}
