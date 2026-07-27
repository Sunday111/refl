#pragma once

#include <deque>
#include <format>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "edt/guid.hpp"
#include "type.hpp"

namespace cppreflection
{
class TypeRegistry;
[[nodiscard]] TypeRegistry* GetTypeRegistry();

template <typename T>
[[nodiscard]] const Type* GetTypeInfo();

class TypeRegistry final
{
public:
    TypeRegistry(const TypeRegistry&) = delete;
    TypeRegistry(TypeRegistry&&) = delete;
    TypeRegistry& operator=(const TypeRegistry&) = delete;
    TypeRegistry& operator=(TypeRegistry&&) = delete;

    [[nodiscard]] const Type* GetTypeInfo(size_t index) const
    {
        const std::lock_guard lock(m_mutex);
        if (index >= m_committed_count)
        {
            throw std::out_of_range("Reflected type index is out of range.");
        }
        return &m_types[index];
    }

    [[nodiscard]] size_t GetTypesCount() const
    {
        const std::lock_guard lock(m_mutex);
        return m_committed_count;
    }

    [[nodiscard]] const Type* FindType(const edt::GUID& guid) const
    {
        const std::lock_guard lock(m_mutex);
        const auto iterator = m_types_by_guid.find(guid);
        return iterator == m_types_by_guid.end() ? nullptr : iterator->second;
    }

    [[nodiscard]] const Type* GetType(const edt::GUID& guid) const
    {
        if (const Type* type = FindType(guid); type != nullptr)
        {
            return type;
        }

        const auto guid_characters = guid.ToCharArray();
        const std::string_view guid_text(guid_characters.data(), guid_characters.size());
        throw std::runtime_error(std::format("Type with guid {} is not registered.", guid_text));
    }

private:
    struct StagedType
    {
        Type* type = nullptr;
        const Type** cache_pointer = nullptr;
    };

    friend TypeRegistry* GetTypeRegistry();

    template <typename T>
    friend const Type* GetTypeInfo();

    TypeRegistry() = default;

    bool BeginReflectionTransaction()
    {
        if (m_transaction_active)
        {
            return false;
        }
        m_transaction_active = true;
        return true;
    }

    Type* StageType(const Type** cache_pointer)
    {
        m_types.emplace_back(Type::ConstructionKey{});
        Type* type = &m_types.back();
        try
        {
            m_staged_types.push_back({type, cache_pointer});
        }
        catch (...)
        {
            m_types.pop_back();
            throw;
        }
        *cache_pointer = type;
        return type;
    }

    [[nodiscard]] size_t GetReflectionTransactionSavepoint() const { return m_staged_types.size(); }

    void CommitReflectionTransaction()
    {
        const edt::GUID zero_guid{};
        for (size_t index = 0; index < m_staged_types.size(); ++index)
        {
            const auto guid = m_staged_types[index].type->GetGuid();
            if (guid == zero_guid)
            {
                continue;
            }
            if (m_types_by_guid.contains(guid))
            {
                ThrowDuplicateGuid(guid);
            }
            for (size_t previous = 0; previous < index; ++previous)
            {
                if (m_staged_types[previous].type->GetGuid() == guid)
                {
                    ThrowDuplicateGuid(guid);
                }
            }
        }

        m_types_by_guid.reserve(m_types_by_guid.size() + m_staged_types.size());

        size_t inserted_guids = 0;
        try
        {
            for (const auto& staged : m_staged_types)
            {
                const auto guid = staged.type->GetGuid();
                if (guid != zero_guid)
                {
                    m_types_by_guid.emplace(guid, staged.type);
                    ++inserted_guids;
                }
            }
        }
        catch (...)
        {
            for (const auto& staged : m_staged_types)
            {
                if (inserted_guids == 0)
                {
                    break;
                }
                const auto guid = staged.type->GetGuid();
                if (guid != zero_guid)
                {
                    m_types_by_guid.erase(guid);
                    --inserted_guids;
                }
            }
            throw;
        }

        m_committed_count = m_types.size();
        m_staged_types.clear();
        m_transaction_active = false;
    }

    void RollbackReflectionTransactionTo(size_t savepoint, bool end_transaction)
    {
        while (m_staged_types.size() > savepoint)
        {
            *m_staged_types.back().cache_pointer = nullptr;
            m_staged_types.pop_back();
            m_types.pop_back();
        }
        if (end_transaction)
        {
            m_transaction_active = false;
        }
    }

    [[noreturn]] static void ThrowDuplicateGuid(const edt::GUID& guid)
    {
        const auto guid_characters = guid.ToCharArray();
        const std::string_view guid_text(guid_characters.data(), guid_characters.size());
        throw std::runtime_error(std::format("Duplicate reflected type GUID {}.", guid_text));
    }

    mutable std::recursive_mutex m_mutex;
    std::deque<Type> m_types;
    size_t m_committed_count = 0;
    std::unordered_map<edt::GUID, Type*> m_types_by_guid;
    bool m_transaction_active = false;
    std::vector<StagedType> m_staged_types;
};

[[nodiscard]] inline TypeRegistry* GetTypeRegistry()
{
    static TypeRegistry registry;
    return &registry;
}

inline bool Type::IsA(edt::GUID type_guid) const
{
    if (m_guid == type_guid)
    {
        return true;
    }

    if (m_flags != TypeFlag::CommonClass && m_flags != TypeFlag::Class)
    {
        return false;
    }

    std::optional<edt::GUID> parent = m_base_class_guid;
    while (parent.has_value())
    {
        if (*parent == type_guid)
        {
            return true;
        }
        parent = GetTypeRegistry()->GetType(*parent)->GetBaseClass();
    }
    return false;
}
}  // namespace cppreflection
