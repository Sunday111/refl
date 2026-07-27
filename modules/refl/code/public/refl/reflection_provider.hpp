#pragma once

#include <concepts>
#include <type_traits>

namespace refl
{
template <typename T>
class TypeReflector;

template <typename T, typename Enable = void>
struct TypeReflectionProvider
{
};
}  // namespace refl

namespace refl::detail
{
template <typename T>
concept TypeIsReflectedWithProvider = requires(T, TypeReflector<T>& rt) {
    { TypeReflectionProvider<T>::ReflectType(rt) } -> std::same_as<void>;
};

template <typename T>
concept HasReflectTypeMethod = requires(T, TypeReflector<T>& rt) {
    { T::ReflectType(rt) } -> std::same_as<void>;
};

}  // namespace refl::detail

namespace refl
{

template <typename T>
concept IsTypeDynamicallyReflected = detail::HasReflectTypeMethod<T> || detail::TypeIsReflectedWithProvider<T>;
}  // namespace refl
