#pragma once

#include "../type.hpp"
#include "class.hpp"

namespace refl
{
template <typename T>
const Type* GetTypeInfo();
}

namespace refl::detail
{
// This function copies compile time class information to runtime object
template <IsStaticClassTypeInfo StaticClassInfoT, typename T>
void StaticToDynamic(StaticClassInfoT static_type_info, TypeReflector<T>& dynamic_type_info)
{
    using SCI = decltype(static_type_info);
    using BaseClass = typename SCI::BaseClass;

    dynamic_type_info.SetName(static_type_info.type_name);
    dynamic_type_info.SetGUID(static_type_info.guid);

    static_type_info.ForEachField(
        [&](auto named_object)
        {
            using NO = decltype(named_object);
            dynamic_type_info.template AddField<NO::GetObject()>(NO::GetName());
        });

    static_type_info.ForEachMethod(
        [&](auto named_object)
        {
            using NO = decltype(named_object);
            dynamic_type_info.template AddMethod<NO::GetObject()>(NO::GetName());
        });

    if constexpr (!std::is_void_v<BaseClass>)
    {
        dynamic_type_info.SetBaseClass(GetTypeInfo<BaseClass>()->GetGuid());
    }
}
}  // namespace refl::detail
