#pragma once

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

    [[nodiscard]] const Type* GetType() const { return m_type; }
    [[nodiscard]] std::string_view GetName() const { return m_name; }
    [[nodiscard]] void* GetValue(void* object) const { return m_getter(object); }

private:
    template <auto pField>
    friend class detail::FieldReflector;

    Field() = default;
    void SetType(const Type* type) { m_type = type; }
    void SetName(std::string_view name) { m_name.assign(name); }
    void SetValueGetter(ValueGetter getter) { m_getter = getter; }

    const Type* m_type = nullptr;
    std::string m_name;
    ValueGetter m_getter = nullptr;
};
}  // namespace refl
