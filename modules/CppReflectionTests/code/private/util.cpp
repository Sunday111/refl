#include <iostream>
#include <string>
#include <string_view>

#include "CppReflection/CallReflectedFunction.hpp"
#include "CppReflection/Reflector/TypeReflector.hpp"
#include "gtest/gtest.h"

void PrintType(const cppreflection::Type* typeInfo, std::ostream& output)
{
    assert(typeInfo->GetName().size() != 0);
    output << typeInfo->GetName().data() << '\n';
    bool needNewLine = false;
    auto methods = typeInfo->GetMethods();
    if (methods.size() > 0)
    {
        output << "   Methods:\n";
        for (const cppreflection::Function* methodInfo : methods)
        {
            output << "      ";

            if (auto objectType = methodInfo->GetObjectType(); objectType == nullptr)
            {
                output << "static ";
            }

            if (auto rv = methodInfo->GetReturnType())
            {
                output << rv->GetName();
            }
            else
            {
                output << "void";
            }

            output << ' ' << methodInfo->GetName() << '(';
            auto arguments = methodInfo->GetArguments();
            for (size_t methodArgIndex = 0; methodArgIndex < arguments.size(); ++methodArgIndex)
            {
                auto methodArgTypeInfo = arguments[methodArgIndex];
                output << methodArgTypeInfo->GetName();
                if (methodArgIndex < arguments.size() - 1)
                {
                    output << ", ";
                }
            }
            output << ");\n";
        }
        needNewLine = true;
    }
    if (needNewLine)
    {
        output << '\n';
    }
}

void PrintReflectedTypes(const cppreflection::TypeRegistry& registry, std::ostream& output)
{
    for (size_t i = 0; i < registry.GetTypesCount(); ++i)
    {
        PrintType(registry.GetTypeInfo(i), output);
    }
}
