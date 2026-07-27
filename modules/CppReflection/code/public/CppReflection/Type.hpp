#pragma once

#include <cassert>
#include <cstddef>
#include <deque>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "edt/guid.hpp"
#include "Field.hpp"
#include "Function.hpp"
#include "TypeComparisonOperators.hpp"
#include "TypeFlag.hpp"
#include "TypeSpecialMembers.hpp"

namespace cppreflection
{
template <typename T>
class TypeReflector;
class TypeRegistry;

class Type final
{
private:
    class ConstructionKey
    {
        friend class TypeRegistry;
        ConstructionKey() = default;
    };

public:
    explicit Type(const ConstructionKey&) {}
    Type(const Type&) = delete;
    Type(Type&&) = delete;
    Type& operator=(const Type&) = delete;
    Type& operator=(Type&&) = delete;

    [[nodiscard]] bool IsA(edt::GUID type_guid) const;
    [[nodiscard]] std::optional<edt::GUID> GetBaseClass() const { return m_base_class_guid; }
    [[nodiscard]] size_t GetInstanceSize() const { return m_instance_size; }
    [[nodiscard]] size_t GetAlignment() const { return m_alignment; }
    [[nodiscard]] std::string_view GetName() const { return m_name; }
    [[nodiscard]] std::span<const Function* const> GetMethods() const { return m_methods; }
    [[nodiscard]] std::span<const Field* const> GetFields() const { return m_fields; }
    [[nodiscard]] TypeFlag GetFlags() const { return m_flags; }
    [[nodiscard]] const TypeSpecialMembers& GetSpecialMembers() const { return m_special_members; }
    [[nodiscard]] const TypeComparisonOperators& GetComparisonOperators() const { return m_comparison_operators; }
    [[nodiscard]] const edt::GUID& GetGuid() const { return m_guid; }

private:
    template <typename T>
    friend class TypeReflector;
    friend class TypeRegistry;

    void SetBaseClass(edt::GUID base_class_guid)
    {
        assert(!m_base_class_guid.has_value());
        m_base_class_guid = base_class_guid;
    }
    void SetAlignment(size_t alignment) { m_alignment = alignment; }
    size_t AddMethod(Function&& function)
    {
        m_owned_methods.push_back(std::move(function));
        try
        {
            m_methods.push_back(&m_owned_methods.back());
        }
        catch (...)
        {
            m_owned_methods.pop_back();
            throw;
        }
        return m_methods.size() - 1;
    }
    size_t AddField(Field&& field)
    {
        m_owned_fields.push_back(std::move(field));
        try
        {
            m_fields.push_back(&m_owned_fields.back());
        }
        catch (...)
        {
            m_owned_fields.pop_back();
            throw;
        }
        return m_fields.size() - 1;
    }
    void SetFlags(TypeFlag flags) { m_flags = flags; }
    void SetInstanceSize(size_t instance_size)
    {
        assert(m_instance_size == 0);
        m_instance_size = instance_size;
    }
    void SetName(std::string_view name) { m_name.assign(name); }
    void SetSpecialMembers(const TypeSpecialMembers& special_members) { m_special_members = special_members; }
    void SetComparisonOperators(const TypeComparisonOperators& comparison_operators)
    {
        m_comparison_operators = comparison_operators;
    }
    void SetGUID(const edt::GUID& guid) { m_guid = guid; }

    edt::GUID m_guid;
    std::optional<edt::GUID> m_base_class_guid;
    TypeFlag m_flags = TypeFlag::None;
    size_t m_instance_size = 0;
    size_t m_alignment = 0;
    std::string m_name;
    std::deque<Field> m_owned_fields;
    std::vector<const Field*> m_fields;
    std::deque<Function> m_owned_methods;
    std::vector<const Function*> m_methods;
    TypeSpecialMembers m_special_members;
    TypeComparisonOperators m_comparison_operators;
};
}  // namespace cppreflection

#include "TypeRegistry.hpp"
