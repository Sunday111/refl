#include <iostream>
#include <string>
#include <string_view>

#include "cpp_reflection/call_reflected_function.hpp"
#include "cpp_reflection/reflector/type_reflector.hpp"
#include "gtest/gtest.h"

namespace FunctionReturnsVoidArgumentVoid_nm
{
class ReflectedType
{
public:
    static void f1() { ++k; }

    static void f2(int arg) { k += arg; }

    static void f3(int* arg) { *arg = 114; }

    static void f4(int& arg) { arg = 115; }

    static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
    {
        rt.SetName("test_function_ret_void_arg_void::ReflectedType");
        rt.AddMethod<&ReflectedType::f1>("f1");
        rt.AddMethod<&ReflectedType::f2>("f2");
        rt.AddMethod<&ReflectedType::f3>("f3");
        rt.AddMethod<&ReflectedType::f4>("f4");
    }

    static int k;
};

int ReflectedType::k = 10;
}  // namespace FunctionReturnsVoidArgumentVoid_nm

TEST(CppReflectionTest, StaticMethod)
{
    using ReflectedType = FunctionReturnsVoidArgumentVoid_nm::ReflectedType;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_function_ret_void_arg_void::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));

    auto methods = typeInfo->GetMethods();
    EXPECT_TRUE(methods.size() == 4);

    {
        const cppreflection::Function* f = methods[0];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("f1"));
        EXPECT_TRUE(f->GetObjectType() == nullptr);
        EXPECT_TRUE(f->GetReturnType() == nullptr);
        EXPECT_TRUE(f->GetArguments().size() == 0);
        cppreflection::CallFunction<void>(f);
        EXPECT_TRUE(ReflectedType::k == 11);
    }

    {
        const cppreflection::Function* f = methods[1];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("f2"));
        EXPECT_TRUE(f->GetObjectType() == nullptr);
        EXPECT_TRUE(f->GetReturnType() == nullptr);
        EXPECT_TRUE(f->GetArguments().size() == 1);
        EXPECT_TRUE(f->GetArguments()[0] == cppreflection::GetTypeInfo<int>());
        cppreflection::CallFunction<void>(f, 2);
        EXPECT_TRUE(ReflectedType::k == 13);
    }

    {
        const cppreflection::Function* f = methods[2];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("f3"));
        EXPECT_TRUE(f->GetObjectType() == nullptr);
        EXPECT_TRUE(f->GetReturnType() == nullptr);
        EXPECT_TRUE(f->GetArguments().size() == 1);
        EXPECT_TRUE(f->GetArguments()[0] == cppreflection::GetTypeInfo<int*>());
        int arg = 3;
        cppreflection::CallFunction<void>(f, &arg);
        EXPECT_TRUE(arg == 114);
    }

    {
        const cppreflection::Function* f = methods[3];
        EXPECT_TRUE(f != nullptr);
        EXPECT_TRUE(f->GetName() == std::string_view("f4"));
        EXPECT_TRUE(f->GetObjectType() == nullptr);
        EXPECT_TRUE(f->GetReturnType() == nullptr);
        EXPECT_TRUE(f->GetArguments().size() == 1);
        EXPECT_TRUE(f->GetArguments()[0] == cppreflection::GetTypeInfo<int&>());
        int arg = 3;
        cppreflection::CallFunction<void>(f, arg);
        EXPECT_TRUE(arg == 115);
    }
}
