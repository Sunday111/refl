#include "CppReflection/CallReflectedFunction.hpp"
#include "CppReflection/Reflector/TypeReflector.hpp"
#include "gtest/gtest.h"

edt::GUID TestGUID()
{
    return edt::GUID::Create("BC16AC85-AD55-4207-99D4-99872369750E");
}

TEST(CppReflectionTest, TypeGUID)
{
    class ReflectedType
    {
    public:
        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt) { rt.SetGUID(TestGUID()); }
    };

    auto typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo->GetGuid() == TestGUID());
}