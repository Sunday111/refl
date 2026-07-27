#pragma once

#include <exception>
#include <string>
#include <type_traits>

#include "detail/call_reflect_type.hpp"
#include "reflector/type_reflector.hpp"
#include "type_registry.hpp"

namespace refl
{
class Type;

namespace detail
{
template <typename T>
struct TypeCache
{
    inline static const Type* pointer = nullptr;
    inline static std::exception_ptr failure;
};
}  // namespace detail

// Recursive reflection for pointer and reference types
template <typename T>
    requires(std::is_pointer_v<T> || std::is_reference_v<T>)
struct TypeReflectionProvider<T, void>
{
    inline static void ReflectType(TypeReflector<T>& rt)
    {
        using Element = std::conditional_t<std::is_pointer_v<T>, std::remove_pointer_t<T>, std::remove_reference_t<T>>;
        using UnqualifiedElement = std::remove_cv_t<Element>;
        auto name = std::string(GetTypeInfo<UnqualifiedElement>()->GetName());
        if constexpr (std::is_const_v<Element>)
        {
            name += " const";
        }
        if constexpr (std::is_volatile_v<Element>)
        {
            name += " volatile";
        }

        if constexpr (std::is_pointer_v<T>)
        {
            name += '*';
            rt.SetName(name);
        }
        else if constexpr (std::is_rvalue_reference_v<T>)
        {
            name += "&&";
            rt.SetName(name);
        }
        else
        {
            name += '&';
            rt.SetName(name);
        }
    }
};

template <typename T>
[[nodiscard]] inline const Type* GetTypeInfo()
{
    using Canonical = std::remove_cv_t<T>;
    if constexpr (!std::is_same_v<T, Canonical>)
    {
        return GetTypeInfo<Canonical>();
    }
    else
    {
        TypeRegistry* registry = GetTypeRegistry();
        const std::lock_guard lock(registry->m_mutex);
        using Cache = detail::TypeCache<T>;
        if (Cache::failure != nullptr)
        {
            std::rethrow_exception(Cache::failure);
        }
        if (Cache::pointer != nullptr)
        {
            return Cache::pointer;
        }

        const bool is_root_transaction = registry->BeginReflectionTransaction();
        const size_t transaction_savepoint = registry->GetReflectionTransactionSavepoint();

        try
        {
            TypeReflector<T> typeReflector(*registry->StageType(&Cache::pointer));
            detail::CallReflectType(typeReflector);
            if (is_root_transaction)
            {
                registry->CommitReflectionTransaction();
            }
        }
        catch (...)
        {
            const auto failure = std::current_exception();
            registry->RollbackReflectionTransactionTo(transaction_savepoint, is_root_transaction);
            Cache::pointer = nullptr;
            Cache::failure = failure;
            throw;
        }
        return Cache::pointer;
    }
}
}  // namespace refl
