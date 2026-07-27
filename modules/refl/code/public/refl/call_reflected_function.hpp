#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

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
            alignas(NoRef) std::array<std::byte, sizeof(NoRef)> rvMemory{};
            fn->CallForwarded(instance, rvMemory.data(), args, categories, argsCount);
            NoRef* pRV = std::launder(reinterpret_cast<NoRef*>(rvMemory.data()));

            struct DestroyOnExit
            {
                NoRef* value;
                ~DestroyOnExit() { std::destroy_at(value); }
            } destroy{pRV};

            NoRef rv(std::move(*pRV));
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
    constexpr size_t argsCount = sizeof...(Args);
    std::array<void*, argsCount> arguments{const_cast<void*>(static_cast<const void*>(std::addressof(args)))...};
    std::array<ArgumentValueCategory, argsCount> categories{
        (std::is_lvalue_reference_v<Args&&> ? ArgumentValueCategory::LValue : ArgumentValueCategory::RValue)...};
    return WrapReflectedFunctionReturnType<ReturnType>(
        fn,
        const_cast<void*>(static_cast<const void*>(std::addressof(instance))),
        arguments.data(),
        categories.data(),
        argsCount);
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
