#pragma once

#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "../detail/member_pointer_owner.hpp"
#include "edt/guid.hpp"
#include "edt/template/static_for.hpp"
#include "edt/template/string_literal.hpp"

namespace refl
{

template <typename T>
class TypeReflector;

template <typename T>
[[nodiscard]] constexpr auto GetStaticTypeInfo();

template <edt::StringLiteral name, auto object, bool ambiguous = false>
struct NamedObject
{
    [[nodiscard]] inline static constexpr std::string_view GetName() { return name.GetView(); }
    [[nodiscard]] static constexpr bool IsAmbiguous() { return ambiguous; }
    [[nodiscard]] static constexpr auto AsAmbiguous() { return NamedObject<name, nullptr, true>{}; }
    [[nodiscard]] inline static constexpr auto GetObject()
        requires(!ambiguous)
    {
        return object;
    }
};

template <NamedObject... objects>
struct NamedObjectsCollection
{
    template <NamedObject new_element>
    [[nodiscard]] static constexpr auto Append()
    {
        if constexpr (((objects.GetName() == new_element.GetName()) || ...))
        {
            return NamedObjectsCollection<[]
                                          {
                                              if constexpr (objects.GetName() == new_element.GetName())
                                              {
                                                  return objects.AsAmbiguous();
                                              }
                                              else
                                              {
                                                  return objects;
                                              }
                                          }()...>{};
        }
        else
        {
            return NamedObjectsCollection<objects..., new_element>{};
        }
    }

    template <NamedObject new_element>
    using Add = decltype(Append<new_element>());

    template <NamedObject... other>
    [[nodiscard]] static constexpr auto Merge()
    {
        if constexpr (sizeof...(other) == 0)
        {
            return NamedObjectsCollection{};
        }
        else
        {
            return []<NamedObject first, NamedObject... rest>
            {
                return decltype(Append<first>())::template Merge<rest...>();
            }.template operator()<other...>();
        }
    }

    template <NamedObject... other>
    [[nodiscard]] constexpr auto operator+(NamedObjectsCollection<other...>) const
    {
        return Merge<other...>();
    }

    template <size_t index>
    using AtIndex = std::tuple_element_t<index, std::tuple<decltype(objects)...>>;

    [[nodiscard]] inline static constexpr bool Contains(std::string_view name)
    {
        return [&]<size_t... indices>(std::index_sequence<indices...>)
        {
            return (false || ... || (AtIndex<indices>{}.GetName() == name));
        }(std::make_index_sequence<sizeof...(objects)>());
    }

    [[nodiscard]] static constexpr size_t Count(std::string_view name)
    {
        return (size_t{0} + ... + (objects.GetName() == name ? size_t{1} : size_t{0}));
    }

    [[nodiscard]] static constexpr bool IsAmbiguous(std::string_view name)
    {
        return ((objects.GetName() == name && objects.IsAmbiguous()) || ...);
    }

    template <edt::StringLiteral name>
        requires(Count(name.GetView()) == 1 && !IsAmbiguous(name.GetView()))
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
    typename Bases = std::tuple<>,
    NamedObjectsCollection methods = NamedObjectsCollection<>{},
    NamedObjectsCollection fields = NamedObjectsCollection<>{},
    NamedObjectsCollection inherited_methods = NamedObjectsCollection<>{},
    NamedObjectsCollection inherited_fields = NamedObjectsCollection<>{}>
struct StaticClassTypeInfo
{
    constexpr StaticClassTypeInfo(std::string_view in_type_name, edt::GUID in_guid)
        : type_name(in_type_name),
          guid(in_guid)
    {
    }

    using BaseClasses = Bases;
    inline static constexpr auto AllMethods = methods + inherited_methods;
    inline static constexpr auto AllFields = fields + inherited_fields;

    template <typename NewBase>
    [[nodiscard]] inline constexpr auto Base() const
    {
        using CurrentMethods = decltype(methods);
        using CurrentFields = decltype(fields);

        static_assert(!std::is_same_v<T, NewBase>);
        static_assert(std::is_convertible_v<T*, NewBase*>, "Reflected bases must be accessible and unambiguous");
        return [&]<typename... Existing>(std::tuple<Existing...>*)
        {
            static_assert((!std::is_same_v<NewBase, Existing> && ...), "A base may only be registered once");
            constexpr auto base = GetStaticTypeInfo<NewBase>();
            return StaticClassTypeInfo<
                T,
                std::tuple<Existing..., NewBase>,
                CurrentMethods{},
                CurrentFields{},
                inherited_methods + base.AllMethods,
                inherited_fields + base.AllFields>(type_name, guid);
        }(static_cast<Bases*>(nullptr));
    }

    template <edt::StringLiteral name, auto method>
        requires(std::is_member_function_pointer_v<decltype(method)> ||
                 std::is_function_v<std::remove_pointer_t<decltype(method)>>) &&
                detail::DeclaredByReflectedType<method, T>
    [[nodiscard]] inline constexpr auto Method() const
    {
        using CurrentMethods = decltype(methods);
        using NewMethods = typename CurrentMethods::template Add<NamedObject<name, method>{}>;
        using CurrentFields = decltype(fields);
        return StaticClassTypeInfo<T, Bases, NewMethods{}, CurrentFields{}, inherited_methods, inherited_fields>(
            type_name,
            guid);
    }

    template <edt::StringLiteral name, auto field>
        requires(std::is_member_object_pointer_v<decltype(field)> || std::is_pointer_v<decltype(field)>) &&
                detail::DeclaredByReflectedType<field, T>
    [[nodiscard]] inline constexpr auto Field() const
    {
        using CurrentMethods = decltype(methods);
        using CurrentFields = decltype(fields);
        using NewFields = typename CurrentFields::template Add<NamedObject<name, field>{}>;
        return StaticClassTypeInfo<T, Bases, CurrentMethods{}, NewFields{}, inherited_methods, inherited_fields>(
            type_name,
            guid);
    }

    template <typename Visitor>
    static constexpr void ForEachBase(Visitor&& visitor)
    {
        [&]<typename... BaseTypes>(std::tuple<BaseTypes...>*)
        {
            (visitor.template operator()<BaseTypes>(), ...);
        }(static_cast<Bases*>(nullptr));
    }

    template <typename Visitor>
    constexpr void ForEachDeclaredField(Visitor&& visitor) const
    {
        fields.ForEach(std::forward<Visitor>(visitor));
    }

    template <typename Visitor>
    constexpr void ForEachDeclaredMethod(Visitor&& visitor) const
    {
        methods.ForEach(std::forward<Visitor>(visitor));
    }

    template <typename Visitor>
    constexpr void ForEachField(Visitor&& visitor) const
    {
        AllFields.ForEach(std::forward<Visitor>(visitor));
    }

    template <typename Visitor>
    constexpr void ForEachMethod(Visitor&& visitor) const
    {
        AllMethods.ForEach(std::forward<Visitor>(visitor));
    }

    template <edt::StringLiteral name>
        requires(AllFields.Count(name.GetView()) == 1 && !AllFields.IsAmbiguous(name.GetView()))
    [[nodiscard]] constexpr auto GetField() const
    {
        return AllFields.template GetByName<name>();
    }

    template <edt::StringLiteral name>
        requires(AllMethods.Count(name.GetView()) == 1 && !AllMethods.IsAmbiguous(name.GetView()))
    [[nodiscard]] constexpr auto GetMethod() const
    {
        return AllMethods.template GetByName<name>();
    }

    [[nodiscard]] constexpr bool HasField(std::string_view name) const { return AllFields.Contains(name); }
    [[nodiscard]] constexpr bool HasMethod(std::string_view name) const { return AllMethods.Contains(name); }

    std::string_view type_name;
    edt::GUID guid;
};

}  // namespace refl

namespace refl::detail
{

template <typename Test>
struct IsStaticClassTypeInfoTrait : std::false_type
{
};

template <
    typename T,
    typename Bases,
    NamedObjectsCollection methods,
    NamedObjectsCollection fields,
    NamedObjectsCollection inherited_methods,
    NamedObjectsCollection inherited_fields>
struct IsStaticClassTypeInfoTrait<StaticClassTypeInfo<T, Bases, methods, fields, inherited_methods, inherited_fields>>
    : std::true_type
{
};

template <typename Test>
inline constexpr bool is_static_class_type_info_v = IsStaticClassTypeInfoTrait<Test>::value;

template <typename Test>
concept IsStaticClassTypeInfo = is_static_class_type_info_v<Test>;
}  // namespace refl::detail
