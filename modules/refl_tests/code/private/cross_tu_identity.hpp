#pragma once

#include "refl/get_type_info.hpp"

struct CrossTranslationUnitType
{
    static void ReflectType(refl::TypeReflector<CrossTranslationUnitType>& reflector)
    {
        reflector.SetName("CrossTranslationUnitType");
        reflector.SetGUID(edt::GUID::Create("6CF3D17C-93F7-4643-830B-CF19D50205F8"));
    }
};

const refl::Type* GetCrossTranslationUnitTypeFromA();
const refl::Type* GetCrossTranslationUnitTypeFromB();
refl::TypeRegistry* GetCrossTranslationUnitRegistryFromA();
refl::TypeRegistry* GetCrossTranslationUnitRegistryFromB();
std::size_t GetCrossTranslationUnitRegistryCountFromA();
const refl::Type* FindCrossTranslationUnitTypeFromB();
