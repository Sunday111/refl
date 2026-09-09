#pragma once

#include "../detail/make_type_comparison_operators.hpp"
#include "../detail/make_type_special_members.hpp"
#include "../detail/member_pointer_owner.hpp"
#include "../reflection_provider.hpp"
#include "../type.hpp"
#include "field_reflector.hpp"
#include "function_reflector.hpp"

namespace refl
{
template <typename T>
[[nodiscard]] constexpr auto GetStaticTypeInfo();

template <typename T>
class TypeReflector
{
public:
    explicit TypeReflector(Type& type);

    void SetName(const std::string_view& name);

    void SetGUID(const edt::GUID& guid);

    template <auto pfn>
        requires detail::DeclaredByReflectedType<pfn, T>
    void AddMethod(const std::string_view& name);

    template <auto pfield>
        requires detail::DeclaredByReflectedType<pfield, T>
    void AddField(const std::string_view& name);

    template <typename Base>
    void SetBaseClass()
    {
        static_assert(!std::is_same_v<std::remove_cvref_t<T>, Base>);
        static_assert(std::is_convertible_v<std::remove_cvref_t<T>*, Base*>);
        m_type->SetBaseClass(GetTypeInfo<Base>());
        AddBaseConversions<Base>();
    }

    void AddAmbiguousField(std::string_view name) { m_type->AddAmbiguousField(name); }
    void AddAmbiguousMethod(std::string_view name) { m_type->AddAmbiguousMethod(name); }

    void SetBaseClass(edt::GUID guid) { m_type->SetBaseClass(guid); }

    Type* GetType() const;

private:
    template <typename Base>
    void AddBaseConversions()
    {
        if constexpr (std::is_convertible_v<std::remove_cvref_t<T>*, Base*>)
        {
            m_type->SetBaseConversion(
                GetTypeInfo<Base>(),
                [](void* instance) -> void*
                { return static_cast<Base*>(static_cast<std::remove_cvref_t<T>*>(instance)); });
        }
        else
        {
            m_type->SetBaseConversion(GetTypeInfo<Base>(), nullptr);
        }
        if constexpr (requires { Base::ReflectType(); } || requires { TypeReflectionProvider<Base>::ReflectType(); })
        {
            GetStaticTypeInfo<Base>().ForEachBase([&]<typename Ancestor> { AddBaseConversions<Ancestor>(); });
        }
    }

    Type* m_type = nullptr;
};

template <typename T>
inline void refl::TypeReflector<T>::SetGUID(const edt::GUID& guid)
{
    m_type->SetGUID(guid);
}

template <typename T>
inline Type* TypeReflector<T>::GetType() const
{
    return m_type;
}

template <typename T>
inline void TypeReflector<T>::SetName(const std::string_view& name)
{
    m_type->SetName(name);
}

template <typename T>
inline TypeReflector<T>::TypeReflector(Type& type) : m_type(&type)
{
    using Value = std::remove_cvref_t<T>;
    m_type->SetSpecialMembers(detail::MakeTypeSpecialMembers<Value>());
    m_type->SetComparisonOperators(detail::MakeTypeComparisonOperators<Value>());
    m_type->SetInstanceSize(sizeof(T));
    m_type->SetAlignment(alignof(T));

    TypeFlag flags = TypeFlag::Common;
    if constexpr (std::is_arithmetic_v<Value>)
    {
        flags |= TypeFlag::Number;
    }
    else if constexpr (std::is_enum_v<Value>)
    {
        flags |= TypeFlag::Enumeration;
    }
    else if constexpr (std::is_class_v<Value> || std::is_union_v<Value>)
    {
        flags |= TypeFlag::Class;
    }
    m_type->SetFlags(flags);
}

template <typename T>
template <auto pfn>
    requires detail::DeclaredByReflectedType<pfn, T>
inline void TypeReflector<T>::AddMethod(const std::string_view& name)
{
    detail::FunctionReflector<pfn> functionReflector;
    functionReflector.SetName(name);
    m_type->AddMethod(functionReflector.TakeFunction());
}

template <typename T>
template <auto pfield>
    requires detail::DeclaredByReflectedType<pfield, T>
inline void TypeReflector<T>::AddField(const std::string_view& name)
{
    detail::FieldReflector<pfield> fieldReflector;
    fieldReflector.SetName(name);
    m_type->AddField(fieldReflector.TakeField());
}
}  // namespace refl
