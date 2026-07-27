#pragma once

#include <cassert>
#include <functional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include "../function.hpp"
#include "edt/template/signature.hpp"

namespace refl
{
template <typename T>
const Type* GetTypeInfo();
}

namespace refl::detail
{
template <auto pfn>
class FunctionReflector
{
    using Signature = edt::SignatureFromPtr<pfn>;

public:
    FunctionReflector();

    Function TakeFunction();

    void SetName(const std::string_view& name);

private:
    template <size_t Index>
    static constexpr decltype(auto) CastArg_i(void** ArgsArray, const ArgumentValueCategory* Categories);

    static void Call(void* Object, void* ReturnValue, void** ArgsArray, size_t ArgsArraySize);

    static void CallForwarded(
        void* Object,
        void* ReturnValue,
        void** ArgsArray,
        const ArgumentValueCategory* Categories,
        size_t ArgsArraySize);

    template <size_t... Index>
    static void Call_i(
        void* Object,
        void* ReturnValue,
        void** ArgsArray,
        const ArgumentValueCategory* Categories,
        size_t ArgsArraySize,
        std::index_sequence<Index...>);

    template <size_t... Index>
    void InitializeArgs(std::index_sequence<Index...>);

    template <size_t Index>
    void InitializeArg();

    void InitializeCaller()
    {
        m_function.SetCaller(Call);
        m_function.SetForwardedCaller(CallForwarded);
    }

private:
    Function m_function;
};

template <auto pfn>
inline FunctionReflector<pfn>::FunctionReflector()
{
    using ReturnType = typename Signature::Ret;
    if constexpr (!std::is_same_v<ReturnType, void>)
    {
        m_function.SetReturnType(GetTypeInfo<ReturnType>());
    }

    if constexpr (!Signature::Pure)
    {
        using Class = typename Signature::Class;
        m_function.SetObjectType(GetTypeInfo<Class>());
    }

    InitializeArgs(std::make_index_sequence<Signature::GetArgsCount()>());
    InitializeCaller();
}
}  // namespace refl::detail

namespace refl::detail
{
template <typename T>
inline constexpr decltype(auto) CastArg_t(void* rawArg, ArgumentValueCategory category)
{
    if constexpr (std::is_reference_v<T>)
    {
        using NoRef = std::remove_reference_t<T>;
        if constexpr (std::is_rvalue_reference_v<T>)
        {
            return std::move(*reinterpret_cast<NoRef*>(rawArg));
        }
        else
        {
            return *reinterpret_cast<NoRef*>(rawArg);
        }
    }
    else
    {
        if (category == ArgumentValueCategory::RValue)
        {
            return T(std::move(*reinterpret_cast<T*>(rawArg)));
        }
        if constexpr (std::is_copy_constructible_v<T>)
        {
            return T(*reinterpret_cast<T*>(rawArg));
        }
        else
        {
            throw std::invalid_argument("A move-only by-value reflected argument requires an rvalue");
        }
    }
}

}  // namespace refl::detail

namespace refl::detail
{
template <auto pfn>
inline void FunctionReflector<pfn>::SetName(const std::string_view& name)
{
    m_function.SetName(name);
}

template <auto pfn>
inline void FunctionReflector<pfn>::Call(void* Object, void* ReturnValue, void** ArgsArray, size_t ArgsArraySize)
{
    Call_i(
        Object,
        ReturnValue,
        ArgsArray,
        nullptr,
        ArgsArraySize,
        std::make_index_sequence<Signature::GetArgsCount()>());
}

template <auto pfn>
inline void FunctionReflector<pfn>::CallForwarded(
    void* Object,
    void* ReturnValue,
    void** ArgsArray,
    const ArgumentValueCategory* Categories,
    size_t ArgsArraySize)
{
    Call_i(
        Object,
        ReturnValue,
        ArgsArray,
        Categories,
        ArgsArraySize,
        std::make_index_sequence<Signature::GetArgsCount()>());
}

template <auto pfn>
template <size_t Index>
inline constexpr decltype(auto) FunctionReflector<pfn>::CastArg_i(
    void** ArgsArray,
    const ArgumentValueCategory* Categories)
{
    using Arguments = typename Signature::Args;
    using T = std::tuple_element_t<Index, Arguments>;
    const auto category = Categories == nullptr ? ArgumentValueCategory::LValue : Categories[Index];
    return CastArg_t<T>(ArgsArray[Index], category);
}

// Make object that will call function or method in the same way
template <typename Class, typename Signature>
decltype(auto) CastObject(void* object)
{
    assert(object != nullptr);
    using ConstClass = std::conditional_t<Signature::Const, const Class, Class>;
    using CvClass = std::conditional_t<Signature::Volatile, volatile ConstClass, ConstClass>;
    auto& instance = *reinterpret_cast<CvClass*>(object);
    if constexpr (Signature::RRef)
    {
        return std::move(instance);
    }
    else
    {
        return (instance);
    }
}

template <auto pfn>
template <size_t... Index>
inline void FunctionReflector<pfn>::Call_i(
    void* Object,
    [[maybe_unused]] void* ReturnValue,
    void** ArgsArray,
    const ArgumentValueCategory* Categories,
    [[maybe_unused]] size_t ArgsArraySize,
    std::index_sequence<Index...>)
{
    assert(ArgsArraySize >= Signature::GetArgsCount());

    using ReturnType = typename Signature::Ret;
    auto call = [&]() -> decltype(auto)
    {
        if constexpr (Signature::Pure)
        {
            return std::invoke(pfn, CastArg_i<Index>(ArgsArray, Categories)...);
        }
        else
        {
            using Class = typename Signature::Class;
            return std::invoke(pfn, CastObject<Class, Signature>(Object), CastArg_i<Index>(ArgsArray, Categories)...);
        }
    };

    if constexpr (std::is_same_v<void, ReturnType>)
    {
        // free function without return type
        call();
    }
    else
    {
        // free function with some return type
        assert(ReturnValue != nullptr);
        if constexpr (std::is_reference_v<ReturnType>)
        {
            using NoRef = std::remove_reference_t<ReturnType>;
            if constexpr (std::is_rvalue_reference_v<ReturnType>)
            {
                auto pRV = reinterpret_cast<NoRef*>(ReturnValue);
                std::construct_at(pRV, std::move(call()));
            }
            else
            {
                // Return type is lvalue reference
                auto ppRV = reinterpret_cast<NoRef**>(ReturnValue);
                *ppRV = &call();
            }
        }
        else
        {
            std::construct_at(reinterpret_cast<ReturnType*>(ReturnValue), call());
        }
    }
}

template <auto pfn>
template <size_t... Index>
inline void FunctionReflector<pfn>::InitializeArgs(std::index_sequence<Index...>)
{
    (InitializeArg<Index>(), ...);
}

template <auto pfn>
template <size_t Index>
inline void FunctionReflector<pfn>::InitializeArg()
{
    using Arguments = typename Signature::Args;
    using Argument = std::tuple_element_t<Index, Arguments>;
    auto typeInfo = GetTypeInfo<Argument>();
    m_function.AddArgumentType(typeInfo);
}

template <auto pfn>
inline Function FunctionReflector<pfn>::TakeFunction()
{
    return std::move(m_function);
}
}  // namespace refl::detail
