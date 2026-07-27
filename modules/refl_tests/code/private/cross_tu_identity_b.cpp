#include "cross_tu_identity.hpp"

const refl::Type* GetCrossTranslationUnitTypeFromB()
{
    return refl::GetTypeInfo<CrossTranslationUnitType>();
}

refl::TypeRegistry* GetCrossTranslationUnitRegistryFromB()
{
    return refl::GetTypeRegistry();
}

const refl::Type* FindCrossTranslationUnitTypeFromB()
{
    const auto* type = refl::GetTypeInfo<CrossTranslationUnitType>();
    return refl::GetTypeRegistry()->FindType(type->GetGuid());
}
