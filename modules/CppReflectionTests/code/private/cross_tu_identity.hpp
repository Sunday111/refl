#pragma once

#include "CppReflection/GetTypeInfo.hpp"

struct CrossTranslationUnitType
{
    static void ReflectType(cppreflection::TypeReflector<CrossTranslationUnitType>& reflector)
    {
        reflector.SetName("CrossTranslationUnitType");
        reflector.SetGUID(edt::GUID::Create("6CF3D17C-93F7-4643-830B-CF19D50205F8"));
    }
};

const cppreflection::Type* GetCrossTranslationUnitTypeFromA();
const cppreflection::Type* GetCrossTranslationUnitTypeFromB();
cppreflection::TypeRegistry* GetCrossTranslationUnitRegistryFromA();
cppreflection::TypeRegistry* GetCrossTranslationUnitRegistryFromB();
std::size_t GetCrossTranslationUnitRegistryCountFromA();
const cppreflection::Type* FindCrossTranslationUnitTypeFromB();
