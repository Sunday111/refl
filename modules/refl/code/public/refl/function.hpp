#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace refl
{
class Type;

namespace detail
{
template <auto pfn>
class FunctionReflector;
}

enum class ArgumentValueCategory
{
    LValue,
    RValue,
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

    [[nodiscard]] const Type* GetReturnType() const { return m_return_type; }
    [[nodiscard]] const Type* GetObjectType() const { return m_object_type; }
    [[nodiscard]] std::span<const Type* const> GetArguments() const { return m_argument_types; }
    [[nodiscard]] std::string_view GetName() const { return m_name; }

    void Call(void* object, void* return_value, void** args, size_t args_count) const
    {
        m_caller(object, return_value, args, args_count);
    }

    void CallForwarded(
        void* object,
        void* return_value,
        void** args,
        const ArgumentValueCategory* categories,
        size_t args_count) const
    {
        m_forwarded_caller(object, return_value, args, categories, args_count);
    }

private:
    template <auto pfn>
    friend class detail::FunctionReflector;

    Function() = default;
    size_t AddArgumentType(const Type* argument_type)
    {
        m_argument_types.push_back(argument_type);
        return m_argument_types.size() - 1;
    }
    void SetObjectType(const Type* object_type) { m_object_type = object_type; }
    void SetCaller(Caller caller) { m_caller = caller; }
    void SetForwardedCaller(ForwardedCaller caller) { m_forwarded_caller = caller; }
    void SetName(std::string_view name) { m_name.assign(name); }
    void SetReturnType(const Type* return_type) { m_return_type = return_type; }

    std::string m_name;
    const Type* m_return_type = nullptr;
    const Type* m_object_type = nullptr;
    std::vector<const Type*> m_argument_types;
    Caller m_caller = nullptr;
    ForwardedCaller m_forwarded_caller = nullptr;
};
}  // namespace refl
