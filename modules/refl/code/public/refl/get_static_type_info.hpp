#pragma once

#include "edt/template/is_specialization.hpp"
#include "reflection_provider.hpp"
#include "reflector/type_reflector.hpp"
#include "static_type/class.hpp"
#include "static_type/enum.hpp"
#include "static_type/fundamental.hpp"
#include "type_registry.hpp"

namespace refl
{

template <typename Test>
concept IsStaticTypeInfoTrait = detail::IsStaticClassTypeInfo<Test> || detail::IsStaticEnumTypeInfo<Test> ||
                                detail::IsStaticFundamentalTypeInfo<Test>;

template <typename T>
concept HasConstexprReflectTypeMethod = requires() {
    { std::remove_cv_t<T>::ReflectType() } -> IsStaticTypeInfoTrait;
};

template <typename T>
concept HasConstexprReflectionProvider = requires() {
    { TypeReflectionProvider<std::remove_cv_t<T>>::ReflectType() } -> IsStaticTypeInfoTrait;
};

template <typename T>
concept HasConstexprRelectionProvider = HasConstexprReflectionProvider<T>;

template <typename T>
concept IsTypeStaticallyReflected = HasConstexprReflectTypeMethod<T> || HasConstexprReflectionProvider<T>;

template <typename T>
[[nodiscard]] inline constexpr auto GetStaticTypeInfo()
{
    static_assert(IsTypeStaticallyReflected<T>);
    using Canonical = std::remove_cv_t<T>;
    if constexpr (HasConstexprReflectTypeMethod<T>)
    {
        return Canonical::ReflectType();
    }
    else
    {
        return TypeReflectionProvider<Canonical>::ReflectType();
    }
}

template <typename T>
[[nodiscard]] inline constexpr edt::GUID GetStaticTypeGUID()
{
    return GetStaticTypeInfo<T>().guid;
}
}  // namespace refl

#include "static_type/class_to_dynamic.hpp"

namespace refl
{

// Some utilities

template <typename Test>
concept IsStaticallyReflectedEnum = requires() {
    { GetStaticTypeInfo<Test>() } -> detail::IsStaticEnumTypeInfo;
};

template <IsStaticallyReflectedEnum T>
[[nodiscard]] inline constexpr std::string_view EnumToString(T value)
{
    return GetStaticTypeInfo<T>().ToString(value);
}

template <IsStaticallyReflectedEnum T>
[[nodiscard]] inline constexpr bool TryParseEnum(std::string_view str, T& out_value)
{
    return GetStaticTypeInfo<T>().TryParse(str, out_value);
}

template <IsStaticallyReflectedEnum T>
[[nodiscard]] inline constexpr std::optional<T> TryParseEnum(std::string_view str)
{
    return GetStaticTypeInfo<T>().TryParse(str);
}

template <IsStaticallyReflectedEnum T>
[[nodiscard]] inline constexpr T ParseEnum(std::string_view str)
{
    return GetStaticTypeInfo<T>().Parse(str);
}

}  // namespace refl
