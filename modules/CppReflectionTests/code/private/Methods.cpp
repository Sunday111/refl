#include <iostream>
#include <string>
#include <string_view>

#include "CppReflection/CallReflectedFunction.hpp"
#include "CppReflection/Reflector/TypeReflector.hpp"
#include "gtest/gtest.h"

TEST(CppReflectionTest, Method_ReturnsVoidArgumentVoid)
{
    class ReflectedType
    {
    public:
        void f1() { ++member; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_void::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int member = 0;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_void_arg_void::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == nullptr);
    EXPECT_TRUE(functionInfo->GetArguments().size() == 0);

    ReflectedType object;
    cppreflection::CallMethod<void>(functionInfo, object);
    EXPECT_TRUE(object.member == 1);
}

TEST(CppReflectionTest, Method_ReturnsVoidArgumentInt)
{
    class ReflectedType
    {
    public:
        void f1(int) {}

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_int::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_void_arg_int::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == nullptr);
    EXPECT_TRUE(functionInfo->GetArguments().size() == 1);
    EXPECT_TRUE(functionInfo->GetArguments()[0] == cppreflection::GetTypeInfo<int>());

    ReflectedType object;
    int arg = 0;
    cppreflection::CallMethod<void>(functionInfo, object, arg);
}

TEST(CppReflectionTest, Method_ReturnsVoidArgumentIntPtr)
{
    class ReflectedType
    {
    public:
        void f1(int* arg) { *arg = member; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_int_ptr::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int member = 123;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_void_arg_int_ptr::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == nullptr);
    EXPECT_TRUE(functionInfo->GetArguments().size() == 1);
    EXPECT_TRUE(functionInfo->GetArguments()[0] == cppreflection::GetTypeInfo<int*>());

    ReflectedType object;
    int arg = 0;
    int* pArg = &arg;
    cppreflection::CallMethod<void>(functionInfo, object, pArg);
    EXPECT_TRUE(arg == object.member);
}

TEST(CppReflectionTest, Method_ReturnsVoidArgumentIntRef)
{
    class ReflectedType
    {
    public:
        void f1(int& arg) { arg = 110; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_int_ref::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_void_arg_int_ref::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == nullptr);
    EXPECT_TRUE(functionInfo->GetArguments().size() == 1);
    EXPECT_TRUE(functionInfo->GetArguments()[0] == cppreflection::GetTypeInfo<int&>());

    ReflectedType object;
    int arg = 0;
    cppreflection::CallMethod<void>(functionInfo, object, arg);
    EXPECT_TRUE(arg == 110);
}

TEST(CppReflectionTest, Method_ReturnsVoidArgumentIntPtrRef)
{
    class ReflectedType
    {
    public:
        void f1(int*& arg) { arg = &member; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_int_ptr_ref::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int member = 111;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_void_arg_int_ptr_ref::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == nullptr);
    EXPECT_TRUE(functionInfo->GetArguments().size() == 1);
    EXPECT_TRUE(functionInfo->GetArguments()[0] == cppreflection::GetTypeInfo<int*&>());

    ReflectedType object;
    int* arg = nullptr;
    cppreflection::CallMethod<void>(functionInfo, object, arg);
    EXPECT_TRUE(arg == &object.member);
}

TEST(CppReflectionTest, Method_ReturnsVoidArgumentRValue)
{
    struct MovableType
    {
        static void ReflectType(cppreflection::TypeReflector<MovableType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_rvalue::MovableType");
        }

        std::vector<int> values;
    };

    class ReflectedType
    {
    public:
        void f1(MovableType&& object) { value = std::move(object); }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_void::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        MovableType value;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_void_arg_void::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == nullptr);
    EXPECT_TRUE(functionInfo->GetArguments().size() == 1);
    EXPECT_TRUE(functionInfo->GetArguments()[0] == cppreflection::GetTypeInfo<MovableType&&>());

    MovableType arg;
    arg.values.resize(10);

    ReflectedType object;
    cppreflection::CallMethod<void>(functionInfo, object, arg);
    EXPECT_TRUE(arg.values.empty());
    EXPECT_TRUE(object.value.values.size() == 10);
}

TEST(CppReflectionTest, Method_ReturnsIntArgumentVoid)
{
    class ReflectedType
    {
    public:
        int f1() { return member; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_int::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int member = 112;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_int::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == cppreflection::GetTypeInfo<int>());
    EXPECT_TRUE(functionInfo->GetArguments().size() == 0);

    ReflectedType object;
    int returnValue = cppreflection::CallMethod<int>(functionInfo, object);
    EXPECT_TRUE(returnValue == object.member);
}

TEST(CppReflectionTest, Method_ReturnsIntPtrArgumentVoid)
{
    class ReflectedType
    {
    public:
        int* f1() { return &member; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_int_ptr::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int member = 112;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_int_ptr::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == cppreflection::GetTypeInfo<int*>());
    EXPECT_TRUE(functionInfo->GetArguments().size() == 0);

    ReflectedType object;
    int* returnValue = cppreflection::CallMethod<int*>(functionInfo, object);
    EXPECT_TRUE(returnValue == &object.member);
}

TEST(CppReflectionTest, Method_ReturnsIntRefArgumentVoid)
{
    class ReflectedType
    {
    public:
        int& f1() { return member; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_int_ref::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int member = 112;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_int_ref::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == cppreflection::GetTypeInfo<int&>());
    EXPECT_TRUE(functionInfo->GetArguments().size() == 0);

    ReflectedType object;
    int& returnValue = cppreflection::CallMethod<int&>(functionInfo, object);
    EXPECT_TRUE(&returnValue == &object.member);
}

TEST(CppReflectionTest, Method_ReturnsIntPtrRefArgumentVoid)
{
    class ReflectedType
    {
    public:
        ReflectedType() { pMember = &member; }

        int*& f1() { return pMember; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_int_ptr_ref::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int* pMember = nullptr;
        int member = 112;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_int_ptr_ref::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == cppreflection::GetTypeInfo<int*&>());
    EXPECT_TRUE(functionInfo->GetArguments().size() == 0);

    ReflectedType object;
    int*& returnValue = cppreflection::CallMethod<int*&>(functionInfo, object);
    EXPECT_TRUE(&returnValue == &object.pMember);
}

TEST(CppReflectionTest, Method_ReturnsRValueArgumentVoid)
{
    struct MovableType
    {
        static void ReflectType(cppreflection::TypeReflector<MovableType>& rt)
        {
            rt.SetName("test_method_ret_rvalue::MovableType");
        }

        MovableType() = default;
        MovableType(MovableType&& another) = default;
        MovableType(const MovableType&) = delete;

        ~MovableType() { EXPECT_TRUE(value.empty()); }

        MovableType& operator=(MovableType&& another) = default;
        MovableType& operator=(const MovableType&) = delete;

        std::vector<int> value;
    };

    class ReflectedType
    {
    public:
        ReflectedType()
        {
            member.value.resize(10);
            std::cout << "ReflectedType() " << '\n';
        }

        MovableType&& f1() { return std::move(member); }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_rvalue::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        MovableType member;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_rvalue::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == cppreflection::GetTypeInfo<MovableType&&>());
    EXPECT_TRUE(functionInfo->GetArguments().size() == 0);

    ReflectedType object;
    MovableType moved =
        cppreflection::WrapReflectedFunctionReturnType<MovableType&&>(functionInfo, &object, nullptr, 0);
    EXPECT_TRUE(moved.value.size() == 10);
    moved.value.clear();
}

TEST(CppReflectionTest, Method_ReturnsVoidArgumentClassType)
{
    class ReflectedType
    {
    public:
        void f1(ReflectedType& arg) { arg.member *= member; }

        static void ReflectType(cppreflection::TypeReflector<ReflectedType>& rt)
        {
            rt.SetName("test_method_ret_void_arg_same_type_ref::ReflectedType");
            rt.AddMethod<&ReflectedType::f1>("f1");
        }

        int member = 12;
    };

    using namespace std::literals;
    const cppreflection::Type* typeInfo = cppreflection::GetTypeInfo<ReflectedType>();
    EXPECT_TRUE(typeInfo != nullptr);
    EXPECT_TRUE(typeInfo->GetName() == std::string_view("test_method_ret_void_arg_same_type_ref::ReflectedType"));
    EXPECT_TRUE(typeInfo->GetInstanceSize() == sizeof(ReflectedType));
    EXPECT_TRUE(typeInfo->GetMethods().size() == 1);
    const cppreflection::Function* functionInfo = typeInfo->GetMethods()[0];
    EXPECT_TRUE(functionInfo != nullptr);
    EXPECT_TRUE(functionInfo->GetName() == std::string_view("f1"));
    EXPECT_TRUE(functionInfo->GetObjectType() == typeInfo);
    EXPECT_TRUE(functionInfo->GetReturnType() == nullptr);
    EXPECT_TRUE(functionInfo->GetArguments().size() == 1);
    EXPECT_TRUE(functionInfo->GetArguments()[0] == cppreflection::GetTypeInfo<ReflectedType&>());

    ReflectedType object;
    ReflectedType arg{11};
    cppreflection::CallMethod<void>(functionInfo, object, arg);
    EXPECT_TRUE(arg.member == 11 * 12);
}
