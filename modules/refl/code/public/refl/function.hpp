#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace refl
{
class Type;

namespace detail
{
template <auto pfn>
class FunctionReflector;
}

struct ArgumentValueCategory
{
    enum class Kind
    {
        LValue,
        RValue,
    };

    Kind kind = Kind::LValue;
    bool is_const : 1 = false;
    bool is_volatile : 1 = false;

    template <typename T>
    static constexpr ArgumentValueCategory From()
    {
        using Value = std::remove_reference_t<T>;
        return {
            .kind = std::is_lvalue_reference_v<T> ? Kind::LValue : Kind::RValue,
            .is_const = std::is_const_v<Value>,
            .is_volatile = std::is_volatile_v<Value>,
        };
    }
};

class Function final
{
public:
    using Caller = void (*)(void* object, void* return_value, void** args, size_t args_count);
    using ForwardedCaller = void (*)(
        void* object,
        void* return_value,
        void** args,
        const ArgumentValueCategory* categories,
        size_t args_count);

    Function(const Function&) = delete;
    Function(Function&&) noexcept = default;
    Function& operator=(const Function&) = delete;
    Function& operator=(Function&&) noexcept = default;

    [[nodiscard]] bool IsAmbiguous() const { return m_ambiguous; }
    [[nodiscard]] const Type* GetReturnType() const { return m_return_type; }
    [[nodiscard]] const Type* GetObjectType() const { return m_object_type; }
    [[nodiscard]] std::span<const Type* const> GetArguments() const { return m_argument_types; }
    [[nodiscard]] std::string_view GetName() const { return m_name; }

    void Call(void* object, void* return_value, void** args, size_t args_count) const
    {
        m_caller(*this, object, return_value, args, args_count);
    }

    void CallForwarded(
        void* object,
        void* return_value,
        void** args,
        const ArgumentValueCategory* categories,
        size_t args_count) const
    {
        m_forwarded_caller(*this, object, return_value, args, categories, args_count);
    }

private:
    using ContextCaller =
        void (*)(const Function& function, void* object, void* return_value, void** args, size_t args_count);
    using ContextForwardedCaller = void (*)(
        const Function& function,
        void* object,
        void* return_value,
        void** args,
        const ArgumentValueCategory* categories,
        size_t args_count);

    template <auto pfn>
    friend class detail::FunctionReflector;

    friend class Type;

    Function() = default;
    Function(const Function& source, const Type* owner, void* (*base_cast)(void*))
        : m_name(source.m_name),
          m_return_type(source.m_return_type),
          m_object_type(source.m_object_type == nullptr ? nullptr : owner),
          m_argument_types(source.m_argument_types),
          m_declaring_type(source.m_declaring_type),
          m_declared_caller(source.m_declared_caller),
          m_declared_forwarded_caller(source.m_declared_forwarded_caller),
          m_base_cast(base_cast),
          m_ambiguous(source.m_ambiguous)
    {
        m_caller = [](const Function& fn, void* object, void* result, void** args, size_t count)
        {
            fn.m_declared_caller(
                fn,
                object == nullptr || fn.m_base_cast == nullptr ? object : fn.m_base_cast(object),
                result,
                args,
                count);
        };
        m_forwarded_caller = [](const Function& fn,
                                void* object,
                                void* result,
                                void** args,
                                const ArgumentValueCategory* categories,
                                size_t count)
        {
            fn.m_declared_forwarded_caller(
                fn,
                object == nullptr || fn.m_base_cast == nullptr ? object : fn.m_base_cast(object),
                result,
                args,
                categories,
                count);
        };
    }
    void MarkAmbiguous()
    {
        m_ambiguous = true;
        m_return_type = nullptr;
        m_object_type = nullptr;
        m_declaring_type = nullptr;
        m_argument_types.clear();
        m_base_cast = nullptr;
        SetCaller([](const Function&, void*, void*, void**, size_t)
                  { throw std::invalid_argument("Ambiguous reflected method"); });
        SetForwardedCaller([](const Function&, void*, void*, void**, const ArgumentValueCategory*, size_t)
                           { throw std::invalid_argument("Ambiguous reflected method"); });
    }
    size_t AddArgumentType(const Type* argument_type)
    {
        m_argument_types.push_back(argument_type);
        return m_argument_types.size() - 1;
    }
    void SetObjectType(const Type* object_type)
    {
        m_object_type = object_type;
        m_declaring_type = object_type;
    }
    void SetCaller(ContextCaller caller)
    {
        m_caller = caller;
        m_declared_caller = caller;
    }
    void SetForwardedCaller(ContextForwardedCaller caller)
    {
        m_forwarded_caller = caller;
        m_declared_forwarded_caller = caller;
    }
    void SetName(std::string_view name) { m_name.assign(name); }
    void SetReturnType(const Type* return_type) { m_return_type = return_type; }

    std::string m_name;
    const Type* m_return_type = nullptr;
    const Type* m_object_type = nullptr;
    std::vector<const Type*> m_argument_types;
    const Type* m_declaring_type = nullptr;
    ContextCaller m_declared_caller = nullptr;
    ContextForwardedCaller m_declared_forwarded_caller = nullptr;
    void* (*m_base_cast)(void*) = nullptr;
    bool m_ambiguous = false;
    ContextCaller m_caller = nullptr;
    ContextForwardedCaller m_forwarded_caller = nullptr;
};
}  // namespace refl
