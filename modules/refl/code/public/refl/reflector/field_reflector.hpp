#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

#include "../detail/field_traits.hpp"
#include "../field.hpp"

namespace refl
{
template <typename T>
const Type* GetTypeInfo();
}

namespace refl::detail
{
template <auto pField>
class FieldReflector
{
public:
    using FieldTraits = FieldPointerTraits<pField>;

    FieldReflector()
    {
        using Traits = FieldPointerTraits<pField>;
        using Field = typename Traits::Field;
        m_field.SetType(GetTypeInfo<Field>());
        m_field.SetConst(std::is_const_v<Field>);

        if constexpr (Traits::IsStatic())
        {
            m_field.SetValueGetter(
                [](const refl::Field&, [[maybe_unused]] const void* instance) -> const void* { return pField; });
        }
        else
        {
            m_field.SetDeclaringType(GetTypeInfo<typename Traits::Class>());
            m_field.SetValueGetter(
                [](const refl::Field&, const void* instance) -> const void*
                {
                    assert(instance != nullptr);
                    auto casted = static_cast<const typename Traits::Class*>(instance);
                    return &(casted->*pField);
                });
        }
    }

    void SetName(const std::string_view& name) { m_field.SetName(name); }

    Field TakeField() { return std::move(m_field); }

private:
    Field m_field;
};
}  // namespace refl::detail
