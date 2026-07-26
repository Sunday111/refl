#include "cross_tu_identity.hpp"

const cppreflection::Type* GetCrossTranslationUnitTypeFromB()
{
    return cppreflection::GetTypeInfo<CrossTranslationUnitType>();
}

cppreflection::TypeRegistry* GetCrossTranslationUnitRegistryFromB()
{
    return cppreflection::GetTypeRegistry();
}

const cppreflection::Type* FindCrossTranslationUnitTypeFromB()
{
    const auto* type = cppreflection::GetTypeInfo<CrossTranslationUnitType>();
    return cppreflection::GetTypeRegistry()->FindType(type->GetGuid());
}
