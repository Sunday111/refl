#include "cross_tu_identity.hpp"

const cppreflection::Type* GetCrossTranslationUnitTypeFromA()
{
    return cppreflection::GetTypeInfo<CrossTranslationUnitType>();
}

cppreflection::TypeRegistry* GetCrossTranslationUnitRegistryFromA()
{
    return cppreflection::GetTypeRegistry();
}

std::size_t GetCrossTranslationUnitRegistryCountFromA()
{
    return cppreflection::GetTypeRegistry()->GetTypesCount();
}
