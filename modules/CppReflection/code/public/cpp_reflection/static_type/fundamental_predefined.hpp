#pragma once

#include <cstdint>

#include "fundamental.hpp"

namespace cppreflection
{
template <>
struct TypeReflectionProvider<int8_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<int8_t>("int8", edt::GUID::Create("0706BBE6-79CC-425C-812E-5ABDDC52DCAD"));
    }
};

template <>
struct TypeReflectionProvider<int16_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<int16_t>("int16", edt::GUID::Create("B88D207C-4BBD-4D81-A70F-F3E9162F044C"));
    }
};

template <>
struct TypeReflectionProvider<int32_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<int32_t>("int32", edt::GUID::Create("F8F358DF-D954-4CED-B7EB-0B76F90459AF"));
    }
};

template <>
struct TypeReflectionProvider<int64_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<int64_t>("int64", edt::GUID::Create("D143D95D-EEF8-4A32-8DB1-FB326B593F76"));
    }
};

template <>
struct TypeReflectionProvider<uint8_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<uint8_t>("uint8", edt::GUID::Create("86D0C889-7FEA-45CE-BA4E-020895E750E4"));
    }
};

template <>
struct TypeReflectionProvider<uint16_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<uint16_t>("uint16", edt::GUID::Create("5AB951D5-91A8-4E2B-B5A8-DE67AA6414F4"));
    }
};

template <>
struct TypeReflectionProvider<uint32_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<uint32_t>("uint32", edt::GUID::Create("3B9AD7E1-C6AF-4091-AA3A-D6B040A4F261"));
    }
};

template <>
struct TypeReflectionProvider<uint64_t>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<uint64_t>("uint64", edt::GUID::Create("F51D1445-C233-4750-86D2-203BBBACB6F1"));
    }
};

template <>
struct TypeReflectionProvider<float>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<float>("float", edt::GUID::Create("3FE50F79-8298-4813-AC20-B4DCCCBEF2F8"));
    }
};

template <>
struct TypeReflectionProvider<double>
{
    [[nodiscard]] inline static constexpr auto ReflectType()
    {
        return StaticFundamentalTypeInfo<double>("double", edt::GUID::Create("F8F090A7-66B8-4593-BEE8-7A695CA4AC25"));
    }
};
}  // namespace cppreflection
