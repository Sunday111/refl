#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace refl
{
class Type;

namespace detail
{
template <auto pField>
class FieldReflector;
}

class Field final
{
public:
    using ValueGetter = void* (*)(void* instance);

    Field(const Field&) = delete;
    Field(Field&&) noexcept = default;
    Field& operator=(const Field&) = delete;
    Field& operator=(Field&&) noexcept = default;

    [[nodiscard]] bool IsAmbiguous() const { return m_ambiguous; }
    [[nodiscard]] const Type* GetType() const { return m_type; }
    [[nodiscard]] std::string_view GetName() const { return m_name; }
    [[nodiscard]] void* GetValue(void* object) const { return m_getter(*this, object); }

private:
    using ContextValueGetter = void* (*)(const Field& field, void* instance);

    template <auto pField>
    friend class detail::FieldReflector;

    friend class Type;

    Field() = default;
    Field(const Field& source, void* (*base_cast)(void*))
        : m_type(source.m_type),
          m_name(source.m_name),
          m_declaring_type(source.m_declaring_type),
          m_declared_getter(source.m_declared_getter),
          m_base_cast(base_cast),
          m_ambiguous(source.m_ambiguous)
    {
        m_getter = [](const Field& field, void* object)
        {
            return field.m_declared_getter(
                field,
                object == nullptr || field.m_base_cast == nullptr ? object : field.m_base_cast(object));
        };
    }
    void MarkAmbiguous()
    {
        m_ambiguous = true;
        m_type = nullptr;
        m_declaring_type = nullptr;
        m_base_cast = nullptr;
        SetValueGetter([](const Field&, void*) -> void* { throw std::invalid_argument("Ambiguous reflected field"); });
    }
    void SetType(const Type* type) { m_type = type; }
    void SetName(std::string_view name) { m_name.assign(name); }
    void SetDeclaringType(const Type* type) { m_declaring_type = type; }
    void SetValueGetter(ContextValueGetter getter)
    {
        m_getter = getter;
        m_declared_getter = getter;
    }

    const Type* m_type = nullptr;
    std::string m_name;
    ContextValueGetter m_getter = nullptr;
    const Type* m_declaring_type = nullptr;
    ContextValueGetter m_declared_getter = nullptr;
    void* (*m_base_cast)(void*) = nullptr;
    bool m_ambiguous = false;
};
}  // namespace refl
