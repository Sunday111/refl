#include "cross_tu_identity.hpp"

const refl::Type* GetCrossTranslationUnitTypeFromA()
{
    return refl::GetTypeInfo<CrossTranslationUnitType>();
}

refl::TypeRegistry* GetCrossTranslationUnitRegistryFromA()
{
    return refl::GetTypeRegistry();
}

std::size_t GetCrossTranslationUnitRegistryCountFromA()
{
    return refl::GetTypeRegistry()->GetTypesCount();
}
