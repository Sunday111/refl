#include <cstdint>
#include <type_traits>

#include "CppReflection/GetStaticTypeInfo.hpp"
#include "gtest/gtest.h"

namespace
{
struct OwnerBase
{
    int field = 0;
    void Method() {}
};

struct OwnerDerived : OwnerBase
{
};
struct ForeignOwner
{
    int field = 0;
    void Method() {}
};

template <auto field>
concept CanAddOwnerField =
    requires { cppreflection::StaticClassTypeInfo<OwnerDerived>("OwnerDerived", {}).template Field<"field", field>(); };

template <auto method>
concept CanAddOwnerMethod = requires {
    cppreflection::StaticClassTypeInfo<OwnerDerived>("OwnerDerived", {}).template Method<"method", method>();
};

template <typename Info>
concept CanFindMissingField = requires(const Info& info) { info.template GetField<"missing">(); };

template <typename Info>
concept CanFindMissingMethod = requires(const Info& info) { info.template GetMethod<"missing">(); };

int ReflectedMethod()
{
    return 1;
}
int DuplicateMethodOne()
{
    return 1;
}
int DuplicateMethodTwo()
{
    return 2;
}

struct StaticInfoHost
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return cppreflection::StaticClassTypeInfo<StaticInfoHost>("StaticInfoHost", {})
            .Method<"ReflectedMethod", &ReflectedMethod>();
    }
};

enum class OptionalEnum : std::uint8_t
{
    One,
    Two
};
}  // namespace

namespace cppreflection
{
template <>
struct TypeReflectionProvider<OptionalEnum>
{
    [[nodiscard]] static constexpr auto ReflectType()
    {
        return StaticEnumTypeInfo<OptionalEnum>(
                   "OptionalEnum",
                   edt::GUID::Create("D59108CD-A7C2-419C-A0F3-8D72846B08C2"))
            .Value(OptionalEnum::One, "One")
            .Value(OptionalEnum::Two, "Two");
    }
};
}  // namespace cppreflection

TEST(StaticTypeInfoTest, SupportsConceptsLookupAndOptionalEnumParsing)
{
    static_assert(cppreflection::IsTypeStaticallyReflected<OptionalEnum>);
    static_assert(cppreflection::IsTypeStaticallyReflected<const OptionalEnum>);
    static_assert(cppreflection::HasConstexprReflectionProvider<OptionalEnum>);
    static_assert(cppreflection::HasConstexprRelectionProvider<OptionalEnum>);
    static_assert(std::is_same_v<
                  decltype(cppreflection::GetStaticTypeInfo<const OptionalEnum>()),
                  decltype(cppreflection::GetStaticTypeInfo<OptionalEnum>())>);

    constexpr auto parsed = cppreflection::TryParseEnum<OptionalEnum>("Two");
    static_assert(parsed.has_value() && *parsed == OptionalEnum::Two);
    static_assert(!cppreflection::TryParseEnum<OptionalEnum>("Missing").has_value());

    constexpr auto info = StaticInfoHost::ReflectType();
    static_assert(info.HasMethod("ReflectedMethod"));
    static_assert(info.GetMethod<"ReflectedMethod">() == &ReflectedMethod);
    static_assert(!CanFindMissingField<decltype(info)>);
    static_assert(!CanFindMissingMethod<decltype(info)>);

    static_assert(CanAddOwnerField<&OwnerBase::field>);
    static_assert(!CanAddOwnerField<&ForeignOwner::field>);
    static_assert(CanAddOwnerMethod<&OwnerBase::Method>);
    static_assert(!CanAddOwnerMethod<&ForeignOwner::Method>);

    constexpr auto duplicate_methods = cppreflection::StaticClassTypeInfo<StaticInfoHost>("DuplicateMethods", {})
                                           .Method<"same", &DuplicateMethodOne>()
                                           .Method<"same", &DuplicateMethodTwo>();
    static_assert(duplicate_methods.HasMethod("same"));
    static_assert(duplicate_methods.GetMethod<"same">() == &DuplicateMethodOne);
}
