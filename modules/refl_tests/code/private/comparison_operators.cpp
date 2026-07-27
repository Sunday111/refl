#include <iostream>
#include <string>
#include <string_view>

#include "refl/call_reflected_function.hpp"
#include "refl/reflector/type_reflector.hpp"
#include "gtest/gtest.h"

TEST(reflTest, ComparisonOperators_LessOperator)
{
    class ReflectedType
    {
    public:
        bool operator<(const ReflectedType& b) const { return member < b.member; }

        static void ReflectType(refl::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_comparison_operators::less_operator::ReflectedType");
        }

        int member = 1024;
    };

    const refl::Type* type = refl::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetComparisonOperators().less != nullptr);

    ReflectedType a;
    a.member = 10;

    ReflectedType b;
    b.member = 20;

    EXPECT_TRUE(type->GetComparisonOperators().less(&a, &b));
    EXPECT_TRUE(!type->GetComparisonOperators().less(&a, &a));
    EXPECT_TRUE(!type->GetComparisonOperators().less(&b, &a));
}

TEST(reflTest, ComparisonOperators_EqualsOperator)
{
    class ReflectedType
    {
    public:
        bool operator==(const ReflectedType& b) const { return member == b.member; }

        static void ReflectType(refl::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_comparison_operators::equals_operator::ReflectedType");
        }

        int member = 1024;
    };

    const refl::Type* type = refl::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetComparisonOperators().equals != nullptr);

    ReflectedType a;
    a.member = 10;

    ReflectedType b;
    b.member = 20;

    EXPECT_TRUE(!type->GetComparisonOperators().equals(&a, &b));
    EXPECT_TRUE(!type->GetComparisonOperators().equals(&b, &a));
    EXPECT_TRUE(type->GetComparisonOperators().equals(&a, &a));
    EXPECT_TRUE(type->GetComparisonOperators().equals(&b, &b));
}

class ReflectedType_NoCompare
{
public:
    static void ReflectType(refl::TypeReflector<ReflectedType_NoCompare>& rt)
    {
        rt.SetName("test_comparison_operators::ReflectedType_NoCompare");
    }
};

TEST(reflTest, ComparisonOperators_NoLessOperator)
{
    const refl::Type* type = refl::GetTypeInfo<ReflectedType_NoCompare>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetComparisonOperators().less == nullptr);
}

TEST(reflTest, ComparisonOperators_NoEqualsOperator)
{
    const refl::Type* type = refl::GetTypeInfo<ReflectedType_NoCompare>();
    EXPECT_TRUE(type != nullptr);
    EXPECT_TRUE(type->GetComparisonOperators().equals == nullptr);
}
