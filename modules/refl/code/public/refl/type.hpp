#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "edt/guid.hpp"
#include "field.hpp"
#include "function.hpp"
#include "type_comparison_operators.hpp"
#include "type_flag.hpp"
#include "type_special_members.hpp"

namespace refl
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

    [[nodiscard]] void* CastTo(const Type* target, void* instance) const
    {
        if (target == this || instance == nullptr)
        {
            return instance;
        }
        const auto conversion = m_base_conversions.find(target);
        return conversion == m_base_conversions.end() || conversion->second == nullptr ? nullptr
                                                                                       : conversion->second(instance);
    }

    [[nodiscard]] bool IsA(edt::GUID type_guid) const;
    [[nodiscard]] std::span<const edt::GUID> GetBaseClasses() const { return m_base_class_guids; }
    [[nodiscard]] const Field* GetField(std::string_view name) const { return FindMember(m_fields, name); }
    [[nodiscard]] const Function* GetMethod(std::string_view name) const { return FindMember(m_methods, name); }
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

    using BaseCast = void* (*)(void*);

    template <typename Member>
    static const Member* FindMember(const std::vector<const Member*>& members, std::string_view name)
    {
        const auto found = std::ranges::find(members, name, &Member::GetName);
        return found == members.end() ? nullptr : *found;
    }

    void SetBaseClass(edt::GUID base_class_guid) { m_base_class_guids.push_back(base_class_guid); }
    void SetBaseClass(const Type* base_type)
    {
        if (std::ranges::find(m_direct_bases, base_type) != m_direct_bases.end())
        {
            throw std::invalid_argument("A base may only be registered once");
        }
        SetBaseClass(base_type->GetGuid());
        m_direct_bases.push_back(base_type);
    }
    void SetBaseConversion(const Type* base_type, BaseCast cast) { m_base_conversions.try_emplace(base_type, cast); }
    void FinalizeInheritedMembers()
    {
        if (m_inheritance_finalized)
        {
            return;
        }
        for (const auto* base : m_direct_bases)
        {
            const_cast<Type*>(base)->FinalizeInheritedMembers();
            for (const auto& [ancestor, conversion] : base->m_base_conversions)
            {
                if (!m_base_conversions.contains(ancestor))
                {
                    throw std::invalid_argument("Inherited hierarchies require compile-time base traits");
                }
            }
            for (const auto* field : base->GetFields())
            {
                const auto cast =
                    field->m_declaring_type == nullptr ? nullptr : m_base_conversions.at(field->m_declaring_type);
                Field inherited(*field, cast);
                if (field->m_declaring_type != nullptr && cast == nullptr)
                {
                    inherited.MarkAmbiguous();
                }
                AddField(std::move(inherited));
            }
            for (const auto* method : base->GetMethods())
            {
                const auto cast =
                    method->m_declaring_type == nullptr ? nullptr : m_base_conversions.at(method->m_declaring_type);
                Function inherited(*method, this, cast);
                if (method->m_declaring_type != nullptr && cast == nullptr)
                {
                    inherited.MarkAmbiguous();
                }
                AddMethod(std::move(inherited));
            }
        }
        m_inheritance_finalized = true;
    }
    void SetAlignment(size_t alignment) { m_alignment = alignment; }
    template <typename Member>
    static size_t AddMember(std::deque<Member>& owned, std::vector<const Member*>& members, Member&& member)
    {
        for (size_t index = 0; index < owned.size(); ++index)
        {
            if (owned[index].GetName() == member.GetName())
            {
                owned[index].MarkAmbiguous();
                return index;
            }
        }
        owned.push_back(std::move(member));
        try
        {
            members.push_back(&owned.back());
        }
        catch (...)
        {
            owned.pop_back();
            throw;
        }
        return members.size() - 1;
    }
    size_t AddMethod(Function&& function) { return AddMember(m_owned_methods, m_methods, std::move(function)); }
    size_t AddField(Field&& field) { return AddMember(m_owned_fields, m_fields, std::move(field)); }
    void AddAmbiguousField(std::string_view name)
    {
        Field field;
        field.SetName(name);
        field.MarkAmbiguous();
        AddField(std::move(field));
    }
    void AddAmbiguousMethod(std::string_view name)
    {
        Function method;
        method.SetName(name);
        method.MarkAmbiguous();
        AddMethod(std::move(method));
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
    std::vector<edt::GUID> m_base_class_guids;
    std::unordered_set<edt::GUID> m_ancestor_guids;
    std::vector<const Type*> m_direct_bases;
    std::unordered_map<const Type*, BaseCast> m_base_conversions;
    bool m_inheritance_finalized = false;
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
}  // namespace refl

#include "type_registry.hpp"
