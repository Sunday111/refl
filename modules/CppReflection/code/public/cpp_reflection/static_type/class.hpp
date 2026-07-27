#pragma once

#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "edt/guid.hpp"
#include "edt/template/static_for.hpp"
#include "edt/template/string_literal.hpp"

namespace cppreflection
{

namespace detail
{
template <typename T>
struct MemberPointerOwner;

template <typename Member, typename Class>
struct MemberPointerOwner<Member Class::*>
{
    using Type = Class;
};

template <auto pointer, typename Reflected>
concept BelongsToReflectedHierarchy =
    !std::is_member_pointer_v<decltype(pointer)> ||
    std::is_base_of_v<typename MemberPointerOwner<decltype(pointer)>::Type, Reflected>;
}  // namespace detail

template <typename T>
class TypeReflector;

template <edt::StringLiteral name, auto object>
struct NamedObject
{
    [[nodiscard]] inline static constexpr std::string_view GetName() { return name.GetView(); }
    [[nodiscard]] inline static constexpr auto GetObject() { return object; }
};

template <NamedObject... objects>
struct NamedObjectsCollection
{
    template <NamedObject new_element>
    using Add = NamedObjectsCollection<objects..., new_element>;

    template <size_t index>
    using AtIndex = std::tuple_element_t<index, std::tuple<decltype(objects)...>>;

    [[nodiscard]] inline static constexpr bool Contains(std::string_view name)
    {
        return [&]<size_t... indices>(std::index_sequence<indices...>)
        {
            return (false || ... || (AtIndex<indices>{}.GetName() == name));
        }(std::make_index_sequence<sizeof...(objects)>());
    }

    template <edt::StringLiteral name>
        requires(Contains(name.GetView()))
    [[nodiscard]] inline static constexpr auto GetByName()
    {
        constexpr size_t index = []
        {
            size_t result = sizeof...(objects);
            edt::StaticFor<sizeof...(objects)>(
                [&]<size_t current>
                {
                    if (result == sizeof...(objects) && AtIndex<current>{}.GetName() == name.GetView())
                    {
                        result = current;
                    }
                });
            return result;
        }();
        return std::get<index>(std::tuple{objects...}).GetObject();
    }

    [[nodiscard]] inline static constexpr size_t Size() { return sizeof...(objects); }

    template <typename Visitor>
    static constexpr void ForEach(Visitor&& visitor)
    {
        (visitor(objects), ...);
    }
};

template <
    typename T,
    typename BaseClass_ = void,
    NamedObjectsCollection methods = NamedObjectsCollection<>{},
    NamedObjectsCollection fields = NamedObjectsCollection<>{}>
struct StaticClassTypeInfo
{
    constexpr StaticClassTypeInfo(std::string_view in_type_name, edt::GUID in_guid)
        : type_name(in_type_name),
          guid(in_guid)
    {
    }

    using BaseClass = BaseClass_;

    template <typename NewBase>
    [[nodiscard]] inline constexpr auto Base() const
    {
        using CurrentMethods = decltype(methods);
        using CurrentFields = decltype(fields);

        static_assert(std::is_void_v<BaseClass>);
        return StaticClassTypeInfo<T, NewBase, CurrentMethods{}, CurrentFields{}>(type_name, guid);
    }

    template <edt::StringLiteral name, auto method>
        requires(std::is_member_function_pointer_v<decltype(method)> ||
                 std::is_function_v<std::remove_pointer_t<decltype(method)>>) &&
                detail::BelongsToReflectedHierarchy<method, T>
    [[nodiscard]] inline constexpr auto Method() const
    {
        using CurrentMethods = decltype(methods);
        using NewMethods = typename CurrentMethods::template Add<NamedObject<name, method>{}>;
        using CurrentFields = decltype(fields);
        return StaticClassTypeInfo<T, BaseClass, NewMethods{}, CurrentFields{}>(type_name, guid);
    }

    template <edt::StringLiteral name, auto field>
        requires(std::is_member_object_pointer_v<decltype(field)> || std::is_pointer_v<decltype(field)>) &&
                detail::BelongsToReflectedHierarchy<field, T>
    [[nodiscard]] inline constexpr auto Field() const
    {
        using CurrentMethods = decltype(methods);
        using CurrentFields = decltype(fields);
        static_assert(!CurrentFields::Contains(name.GetView()), "Reflected field names must be unique");
        using NewFields = typename CurrentFields::template Add<NamedObject<name, field>{}>;
        return StaticClassTypeInfo<T, BaseClass, CurrentMethods{}, NewFields{}>(type_name, guid);
    }

    template <edt::StringLiteral name>
        requires(fields.Contains(name.GetView()))
    [[nodiscard]] inline constexpr auto GetField() const
    {
        return fields.template GetByName<name>();
    }

    template <edt::StringLiteral name>
        requires(methods.Contains(name.GetView()))
    [[nodiscard]] inline constexpr auto GetMethod() const
    {
        return methods.template GetByName<name>();
    }

    [[nodiscard]] inline constexpr bool HasField(std::string_view name) const { return fields.Contains(name); }

    [[nodiscard]] inline constexpr bool HasMethod(std::string_view name) const { return methods.Contains(name); }

    template <typename Visitor>
    constexpr void ForEachField(Visitor&& visitor) const
    {
        fields.ForEach(std::forward<Visitor>(visitor));
    }

    template <typename Visitor>
    constexpr void ForEachMethod(Visitor&& visitor) const
    {
        methods.ForEach(std::forward<Visitor>(visitor));
    }

    std::string_view type_name;
    edt::GUID guid;
};

}  // namespace cppreflection

namespace cppreflection::detail
{

template <typename Test>
struct IsStaticClassTypeInfoTrait : std::false_type
{
};

template <typename T, typename BaseClass, NamedObjectsCollection methods, NamedObjectsCollection fields>
struct IsStaticClassTypeInfoTrait<StaticClassTypeInfo<T, BaseClass, methods, fields>> : std::true_type
{
};

template <typename Test>
inline constexpr bool is_static_class_type_info_v = IsStaticClassTypeInfoTrait<Test>::value;

template <typename Test>
concept IsStaticClassTypeInfo = is_static_class_type_info_v<Test>;
}  // namespace cppreflection::detail
