#pragma once

#include "../detail/make_type_comparison_operators.hpp"
#include "../detail/make_type_special_members.hpp"
#include "../type.hpp"
#include "field_reflector.hpp"
#include "function_reflector.hpp"

namespace cppreflection
{
template <typename T>
class TypeReflector
{
public:
    explicit TypeReflector(Type& type);

    void SetName(const std::string_view& name);

    void SetGUID(const edt::GUID& guid);

    template <auto pfn>
    void AddMethod(const std::string_view& name);

    template <auto pfield>
    void AddField(const std::string_view& name);

    void SetBaseClass(edt::GUID guid) { m_type->SetBaseClass(guid); }

    Type* GetType() const;

private:
    Type* m_type = nullptr;
};

template <typename T>
inline void cppreflection::TypeReflector<T>::SetGUID(const edt::GUID& guid)
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
        flags = TypeFlag::CommonNumber;
    }
    else if constexpr (std::is_enum_v<Value>)
    {
        flags = TypeFlag::CommonEnumeration;
    }
    else if constexpr (std::is_class_v<Value> || std::is_union_v<Value>)
    {
        flags = TypeFlag::CommonClass;
    }
    m_type->SetFlags(flags);
}

template <typename T>
template <auto pfn>
inline void TypeReflector<T>::AddMethod(const std::string_view& name)
{
    using Signature = edt::SignatureFromPtr<pfn>;
    if constexpr (!Signature::Pure)
    {
        static_assert(
            std::is_base_of_v<typename Signature::Class, std::remove_cvref_t<T>>,
            "Reflected methods must belong to the reflected type or one of its "
            "base classes");
    }
    detail::FunctionReflector<pfn> functionReflector;
    functionReflector.SetName(name);
    m_type->AddMethod(functionReflector.TakeFunction());
}

template <typename T>
template <auto pfield>
inline void TypeReflector<T>::AddField(const std::string_view& name)
{
    using Traits = detail::FieldPointerTraits<pfield>;
    if constexpr (!Traits::IsStatic())
    {
        static_assert(
            std::is_base_of_v<typename Traits::Class, std::remove_cvref_t<T>>,
            "Reflected fields must belong to the reflected type or one of its "
            "base classes");
    }
    detail::FieldReflector<pfield> fieldReflector;
    fieldReflector.SetName(name);
    m_type->AddField(fieldReflector.TakeField());
}
}  // namespace cppreflection
