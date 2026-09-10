#include <iostream>
#include <string>
#include <string_view>

#include "gtest/gtest.h"
#include "refl/call_reflected_function.hpp"
#include "refl/reflector/type_reflector.hpp"

namespace reflTest_Fields
{
class ReflectedType
{
public:
    static void ReflectType(refl::TypeReflector<ReflectedType>& rt)
    {
        rt.SetName("test_field_value::ReflectedType");
        rt.AddField<&ReflectedType::m_int>("m_int");
        rt.AddField<&ReflectedType::m_int_ptr>("m_int_ptr");
        rt.AddField<&ReflectedType::ms_int>("ms_int");
        rt.AddField<&ReflectedType::ms_int_ptr>("ms_int_ptr");
    }

    explicit ReflectedType(int value) : m_int(value), m_int_ptr(&m_int) {}

    int m_int;
    int* m_int_ptr;

    static int ms_int;
    static int* ms_int_ptr;
    static int& ms_int_ref;
};

int ReflectedType::ms_int = 12;
int* ReflectedType::ms_int_ptr = &ReflectedType::ms_int;
}  // namespace reflTest_Fields

TEST(reflTest, Fields)
{
    using ReflectedType = reflTest_Fields::ReflectedType;
    const refl::Type* typeInfo = refl::GetTypeInfo<ReflectedType>();
    ASSERT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_field_value::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetFields().size() == 4);

    ReflectedType instance(100);
    auto fields = typeInfo->GetFields();
    // Reflected int
    {
        auto f = fields[0];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("m_int"));
        EXPECT_TRUE(f->GetType() == refl::GetTypeInfo<int>());
        EXPECT_TRUE(f->GetValue(&instance) == &instance.m_int);
    }

    // Reflected int*
    {
        auto f = fields[1];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("m_int_ptr"));
        EXPECT_TRUE(f->GetType() == refl::GetTypeInfo<int*>());
        EXPECT_TRUE(f->GetValue(&instance) == &instance.m_int_ptr);
    }

    // Reflected static int
    {
        auto f = fields[2];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("ms_int"));
        EXPECT_TRUE(f->GetType() == refl::GetTypeInfo<int>());
        EXPECT_TRUE(f->GetValue(nullptr) == &instance.ms_int);
    }

    // Reflected static int*
    {
        auto f = fields[3];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("ms_int_ptr"));
        EXPECT_TRUE(f->GetType() == refl::GetTypeInfo<int*>());
        EXPECT_TRUE(f->GetValue(nullptr) == &instance.ms_int_ptr);
    }
}

namespace
{
struct QualifiedFields
{
    int writable = 11;
    const int constant = 17;
    const int* pointer = &constant;
    int* const constant_pointer = &writable;
    inline static const int static_constant = 23;
    inline static int static_writable = 29;

    static void ReflectType(refl::TypeReflector<QualifiedFields>& reflector)
    {
        reflector.SetName("QualifiedFields");
        reflector.AddField<&QualifiedFields::writable>("writable");
        reflector.AddField<&QualifiedFields::constant>("constant");
        reflector.AddField<&QualifiedFields::pointer>("pointer");
        reflector.AddField<&QualifiedFields::constant_pointer>("constant_pointer");
        reflector.AddField<&QualifiedFields::static_constant>("static_constant");
        reflector.AddField<&QualifiedFields::static_writable>("static_writable");
    }
};

struct DerivedFields : virtual QualifiedFields
{
    static void ReflectType(refl::TypeReflector<DerivedFields>& reflector)
    {
        reflector.SetName("DerivedFields");
        reflector.SetBaseClass<QualifiedFields>();
    }
};
}  // namespace

TEST(reflTest, QualifiedFields)
{
    const auto* type = refl::GetTypeInfo<QualifiedFields>();
    QualifiedFields instance;
    const QualifiedFields constant_instance;
    const auto* writable = type->GetField("writable");
    ASSERT_NE(writable, nullptr);
    EXPECT_FALSE(writable->IsConst());
    EXPECT_EQ(writable->GetConstValue(&constant_instance), &constant_instance.writable);
    *static_cast<int*>(writable->GetValue(&instance)) = 31;
    EXPECT_EQ(instance.writable, 31);

    const auto* constant = type->GetField("constant");
    ASSERT_NE(constant, nullptr);
    EXPECT_TRUE(constant->IsConst());
    EXPECT_EQ(constant->GetType(), refl::GetTypeInfo<int>());
    EXPECT_EQ(constant->GetConstValue(&constant_instance), &constant_instance.constant);
    EXPECT_EQ(*static_cast<const int*>(constant->GetConstValue(&instance)), 17);
    EXPECT_THROW(static_cast<void>(constant->GetValue(&instance)), std::invalid_argument);

    const auto* pointer = type->GetField("pointer");
    ASSERT_NE(pointer, nullptr);
    EXPECT_FALSE(pointer->IsConst());
    *static_cast<const int**>(pointer->GetValue(&instance)) = &constant_instance.constant;
    EXPECT_EQ(instance.pointer, &constant_instance.constant);

    const auto* constant_pointer = type->GetField("constant_pointer");
    ASSERT_NE(constant_pointer, nullptr);
    EXPECT_TRUE(constant_pointer->IsConst());
    EXPECT_EQ(constant_pointer->GetConstValue(&instance), &instance.constant_pointer);
    EXPECT_THROW(static_cast<void>(constant_pointer->GetValue(&instance)), std::invalid_argument);

    const auto* static_constant = type->GetField("static_constant");
    ASSERT_NE(static_constant, nullptr);
    EXPECT_TRUE(static_constant->IsConst());
    EXPECT_EQ(static_constant->GetConstValue(nullptr), &QualifiedFields::static_constant);
    EXPECT_THROW(static_cast<void>(static_constant->GetValue(nullptr)), std::invalid_argument);

    const auto* static_writable = type->GetField("static_writable");
    ASSERT_NE(static_writable, nullptr);
    EXPECT_FALSE(static_writable->IsConst());
    EXPECT_EQ(static_writable->GetValue(nullptr), &QualifiedFields::static_writable);
}

TEST(reflTest, InheritedQualifiedFields)
{
    const auto* type = refl::GetTypeInfo<DerivedFields>();
    DerivedFields instance;
    const DerivedFields constant_instance;
    const auto* constant = type->GetField("constant");
    ASSERT_NE(constant, nullptr);
    EXPECT_TRUE(constant->IsConst());
    EXPECT_EQ(constant->GetConstValue(&constant_instance), &constant_instance.constant);
    EXPECT_THROW(static_cast<void>(constant->GetValue(&instance)), std::invalid_argument);
    const auto* writable = type->GetField("writable");
    ASSERT_NE(writable, nullptr);
    EXPECT_FALSE(writable->IsConst());
    EXPECT_EQ(writable->GetConstValue(&constant_instance), &constant_instance.writable);
    EXPECT_EQ(writable->GetValue(&instance), &instance.writable);
    const auto* static_constant = type->GetField("static_constant");
    ASSERT_NE(static_constant, nullptr);
    EXPECT_TRUE(static_constant->IsConst());
    EXPECT_EQ(static_constant->GetConstValue(nullptr), &QualifiedFields::static_constant);
}
