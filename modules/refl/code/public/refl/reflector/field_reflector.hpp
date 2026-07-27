#pragma once

#include <cassert>
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

        if constexpr (Traits::IsStatic())
        {
            m_field.SetValueGetter(
                []([[maybe_unused]] void* instance) -> void*
                { return const_cast<void*>(static_cast<const void*>(pField)); });
        }
        else
        {
            m_field.SetValueGetter(
                [](void* instance) -> void*
                {
                    assert(instance != nullptr);
                    using Class = typename Traits::Class;
                    auto casted = reinterpret_cast<Class*>(instance);
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
