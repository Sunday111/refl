#include "refl/call_reflected_function.hpp"
#include "refl/reflector/type_reflector.hpp"
#include "gtest/gtest.h"

edt::GUID TestGUID()
{
    return edt::GUID::Create("BC16AC85-AD55-4207-99D4-99872369750E");
}

TEST(reflTest, TypeGUID)
{
    class ReflectedType
    {
    public:
        static void ReflectType(refl::TypeReflector<ReflectedType>& rt) { rt.SetGUID(TestGUID()); }
    };

    auto typeInfo = refl::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo->GetGuid() == TestGUID());
}