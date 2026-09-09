#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "edt/functional/on_scope_leave.hpp"
#include "get_type_info.hpp"

namespace refl
{
template <typename ReturnType>
decltype(auto) WrapReflectedFunctionReturnType(
    const Function* fn,
    void* instance,
    void** args,
    const ArgumentValueCategory* categories,
    size_t argsCount)
{
    if (fn->IsAmbiguous())
    {
        throw std::invalid_argument("Ambiguous reflected method");
    }
    if constexpr (!std::is_same_v<void, ReturnType>)
    {
        assert(GetTypeInfo<ReturnType>() == fn->GetReturnType());
        if constexpr (std::is_lvalue_reference_v<ReturnType>)
        {
            using NoRef = std::remove_reference_t<ReturnType>;
            NoRef* pRV = nullptr;
            fn->CallForwarded(instance, &pRV, args, categories, argsCount);
            assert(pRV != nullptr);
            return static_cast<ReturnType>(*pRV);
        }
        else
        {
            using NoRef = std::remove_reference_t<ReturnType>;
            union ReturnStorage
            {
                ReturnStorage() noexcept {}
                ~ReturnStorage() noexcept {}
                NoRef value;
            } storage;
            fn->CallForwarded(instance, std::addressof(storage.value), args, categories, argsCount);

            auto destroy = edt::OnScopeLeave([&] { std::destroy_at(std::addressof(storage.value)); });

            NoRef rv(std::move(storage.value));
            return rv;
        }
    }
    else
    {
        fn->CallForwarded(instance, nullptr, args, categories, argsCount);
    }
}

template <typename ReturnType>
decltype(auto) WrapReflectedFunctionReturnType(const Function* fn, void* instance, void** args, size_t argsCount)
{
    return WrapReflectedFunctionReturnType<ReturnType>(fn, instance, args, nullptr, argsCount);
}

template <typename ReturnType, typename Class, typename... Args>
decltype(auto) CallMethod(const Function* fn, Class& instance, Args&&... args)
{
    void* object = const_cast<void*>(static_cast<const void*>(std::addressof(instance)));
    if (fn->GetObjectType() != nullptr)
    {
        object = GetTypeInfo<Class>()->CastTo(fn->GetObjectType(), object);
        if (object == nullptr)
        {
            throw std::invalid_argument("The instance cannot be converted to the reflected method's object type");
        }
    }
    constexpr size_t argsCount = sizeof...(Args);
    std::array<void*, argsCount> arguments{const_cast<void*>(static_cast<const void*>(std::addressof(args)))...};
    std::array<ArgumentValueCategory, argsCount> categories{
        (std::is_lvalue_reference_v<Args&&> ? ArgumentValueCategory::LValue : ArgumentValueCategory::RValue)...};
    return WrapReflectedFunctionReturnType<ReturnType>(fn, object, arguments.data(), categories.data(), argsCount);
}

template <typename ReturnType, typename... Args>
decltype(auto) CallFunction(const Function* fn, Args&&... args)
{
    constexpr size_t argsCount = sizeof...(Args);
    std::array<void*, argsCount> arguments{const_cast<void*>(static_cast<const void*>(std::addressof(args)))...};
    std::array<ArgumentValueCategory, argsCount> categories{
        (std::is_lvalue_reference_v<Args&&> ? ArgumentValueCategory::LValue : ArgumentValueCategory::RValue)...};
    return WrapReflectedFunctionReturnType<ReturnType>(fn, nullptr, arguments.data(), categories.data(), argsCount);
}

}  // namespace refl
