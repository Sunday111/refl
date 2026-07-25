#pragma once

namespace cppreflection::detail
{
template <typename T>
struct FieldTypeTraits
{
};

template <typename Field_, typename Class_>
struct FieldTypeTraits<Field_ Class_::*>
{
    using Field = Field_;
    using Class = Class_;

    [[nodiscard]]
    static constexpr bool IsStatic()
    {
        return false;
    }
};

template <typename Field_>
struct FieldTypeTraits<Field_*>
{
    using Field = Field_;

    [[nodiscard]]
    static constexpr bool IsStatic()
    {
        return true;
    }
};

template <auto pfield>
struct FieldPointerTraits : public FieldTypeTraits<decltype(pfield)>
{
};
}  // namespace cppreflection::detail
