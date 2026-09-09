#include "gtest/gtest.h"
#include "refl/get_static_type_info.hpp"
#include "refl/get_type_info.hpp"

namespace
{
struct BaseType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<BaseType>(
            "BaseType",
            edt::GUID::Create("57630F4D-54C8-43CA-A0F8-540967C4C06B"));
    }
};

struct MiddleType : BaseType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<MiddleType>(
                   "MiddleType",
                   edt::GUID::Create("0E44E448-0BBF-4501-8706-5957068485E0"))
            .Base<BaseType>();
    }
};

struct DerivedType : MiddleType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<DerivedType>(
                   "DerivedType",
                   edt::GUID::Create("91FC8A42-E128-4465-81B0-43ED67D3B7F5"))
            .Base<MiddleType>();
    }
};

struct UnrelatedType
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return refl::StaticClassTypeInfo<UnrelatedType>(
            "UnrelatedType",
            edt::GUID::Create("5E057529-328C-4245-8DE1-E17146CB9EB4"));
    }
};
struct LegacyMiddleType : BaseType
{
    static void ReflectType(refl::TypeReflector<LegacyMiddleType>& rt)
    {
        rt.SetGUID(edt::GUID::Create("9501B9E9-BFAB-4585-B5B6-501EE096C1A7"));
        rt.SetBaseClass(refl::GetTypeInfo<BaseType>()->GetGuid());
    }
};

struct LegacyDerivedType : LegacyMiddleType
{
    static void ReflectType(refl::TypeReflector<LegacyDerivedType>& rt)
    {
        rt.SetGUID(edt::GUID::Create("F1D59079-0A17-4B87-AD3A-BA235FADDE53"));
        rt.SetBaseClass(refl::GetTypeInfo<LegacyMiddleType>()->GetGuid());
    }
};
}  // namespace

TEST(TypeHierarchyTest, TraversesRegisteredBaseTypes)
{
    const auto* derived = refl::GetTypeInfo<DerivedType>();
    const auto* middle = refl::GetTypeInfo<MiddleType>();
    const auto* base = refl::GetTypeInfo<BaseType>();
    const auto* unrelated = refl::GetTypeInfo<UnrelatedType>();
    EXPECT_TRUE(derived->IsA(derived->GetGuid()));
    EXPECT_TRUE(derived->IsA(middle->GetGuid()));
    EXPECT_TRUE(derived->IsA(base->GetGuid()));
    EXPECT_FALSE(base->IsA(derived->GetGuid()));
    EXPECT_FALSE(derived->IsA(unrelated->GetGuid()));
}

TEST(TypeHierarchyTest, ResolvesGuidOnlyAncestryDuringRegistration)
{
    const auto* derived = refl::GetTypeInfo<LegacyDerivedType>();
    EXPECT_TRUE(derived->IsA(refl::GetTypeInfo<LegacyMiddleType>()->GetGuid()));
    EXPECT_TRUE(derived->IsA(refl::GetTypeInfo<BaseType>()->GetGuid()));
    EXPECT_FALSE(derived->IsA(refl::GetTypeInfo<UnrelatedType>()->GetGuid()));
}
