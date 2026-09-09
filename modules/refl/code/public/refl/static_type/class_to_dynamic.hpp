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
    dynamic_type_info.SetName(static_type_info.type_name);
    dynamic_type_info.SetGUID(static_type_info.guid);

    static_type_info.ForEachDeclaredField(
        [&](auto named_object)
        {
            using NO = decltype(named_object);
            if constexpr (NO::IsAmbiguous())
            {
                dynamic_type_info.AddAmbiguousField(NO::GetName());
            }
            else
            {
                dynamic_type_info.template AddField<NO::GetObject()>(NO::GetName());
            }
        });

    static_type_info.ForEachDeclaredMethod(
        [&](auto named_object)
        {
            using NO = decltype(named_object);
            if constexpr (NO::IsAmbiguous())
            {
                dynamic_type_info.AddAmbiguousMethod(NO::GetName());
            }
            else
            {
                dynamic_type_info.template AddMethod<NO::GetObject()>(NO::GetName());
            }
        });

    static_type_info.ForEachBase([&]<typename Base> { dynamic_type_info.template SetBaseClass<Base>(); });
}
}  // namespace refl::detail
