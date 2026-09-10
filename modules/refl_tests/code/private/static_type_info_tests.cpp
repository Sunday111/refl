#include <cstdint>
#include <type_traits>

#include "gtest/gtest.h"
#include "refl/get_static_type_info.hpp"

namespace
{
struct OwnerBase
{
    int field = 0;
    void Method() {}
};

struct OwnerDerived : OwnerBase
{
    int own_field = 0;
    void OwnMethod() {}
};
struct ForeignOwner
{
    int field = 0;
    void Method() {}
};

template <auto field>
concept CanAddOwnerField =
    requires { refl::StaticClassTypeInfo<OwnerDerived>("OwnerDerived", {}).template Field<"field", field>(); };

template <auto method>
concept CanAddOwnerMethod =
    requires { refl::StaticClassTypeInfo<OwnerDerived>("OwnerDerived", {}).template Method<"method", method>(); };

template <auto field>
concept CanAddRuntimeOwnerField =
    requires(refl::TypeReflector<OwnerDerived>& rt) { rt.template AddField<field>("field"); };

template <auto method>
concept CanAddRuntimeOwnerMethod =
    requires(refl::TypeReflector<OwnerDerived>& rt) { rt.template AddMethod<method>("method"); };

template <typename Info>
concept CanFindDuplicateMethod = requires(const Info& info) { info.template GetMethod<"same">(); };

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
        return refl::StaticClassTypeInfo<StaticInfoHost>("StaticInfoHost", {})
            .Method<"ReflectedMethod", &ReflectedMethod>();
    }
};

enum class OptionalEnum : std::uint8_t
{
    One,
    Two
};
}  // namespace

namespace refl
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
}  // namespace refl

TEST(StaticTypeInfoTest, SupportsConceptsLookupAndOptionalEnumParsing)
{
    static_assert(refl::IsTypeStaticallyReflected<OptionalEnum>);
    static_assert(refl::IsTypeStaticallyReflected<const OptionalEnum>);
    static_assert(refl::HasConstexprReflectionProvider<OptionalEnum>);
    static_assert(refl::HasConstexprRelectionProvider<OptionalEnum>);
    static_assert(std::is_same_v<
                  decltype(refl::GetStaticTypeInfo<const OptionalEnum>()),
                  decltype(refl::GetStaticTypeInfo<OptionalEnum>())>);

    constexpr auto parsed = refl::TryParseEnum<OptionalEnum>("Two");
    static_assert(parsed.has_value() && *parsed == OptionalEnum::Two);
    static_assert(!refl::TryParseEnum<OptionalEnum>("Missing").has_value());

    constexpr auto info = StaticInfoHost::ReflectType();
    static_assert(info.HasMethod("ReflectedMethod"));
    static_assert(info.GetMethod<"ReflectedMethod">() == &ReflectedMethod);
    static_assert(!CanFindMissingField<decltype(info)>);
    static_assert(!CanFindMissingMethod<decltype(info)>);

    static_assert(CanAddOwnerField<&OwnerDerived::own_field>);
    static_assert(!CanAddOwnerField<&OwnerBase::field>);
    static_assert(!CanAddOwnerField<&OwnerDerived::field>);
    static_assert(!CanAddOwnerField<&ForeignOwner::field>);
    static_assert(CanAddOwnerMethod<&OwnerDerived::OwnMethod>);
    static_assert(!CanAddOwnerMethod<&OwnerBase::Method>);
    static_assert(!CanAddOwnerMethod<&OwnerDerived::Method>);
    static_assert(!CanAddOwnerMethod<&ForeignOwner::Method>);
    static_assert(CanAddRuntimeOwnerField<&OwnerDerived::own_field>);
    static_assert(!CanAddRuntimeOwnerField<&OwnerBase::field>);
    static_assert(!CanAddRuntimeOwnerField<&OwnerDerived::field>);
    static_assert(!CanAddRuntimeOwnerField<&ForeignOwner::field>);
    static_assert(CanAddRuntimeOwnerMethod<&OwnerDerived::OwnMethod>);
    static_assert(!CanAddRuntimeOwnerMethod<&OwnerBase::Method>);
    static_assert(!CanAddRuntimeOwnerMethod<&OwnerDerived::Method>);
    static_assert(!CanAddRuntimeOwnerMethod<&ForeignOwner::Method>);

    constexpr auto duplicate_methods = refl::StaticClassTypeInfo<StaticInfoHost>("DuplicateMethods", {})
                                           .Method<"same", &DuplicateMethodOne>()
                                           .Method<"same", &DuplicateMethodTwo>();
    static_assert(duplicate_methods.HasMethod("same"));
    static_assert(duplicate_methods.AllMethods.Size() == 1);
    static_assert(duplicate_methods.AllMethods.IsAmbiguous("same"));
    static_assert(!CanFindDuplicateMethod<decltype(duplicate_methods)>);
}
