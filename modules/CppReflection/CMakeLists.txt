cmake_minimum_required(VERSION 3.20)
include(set_compiler_options)
set(module_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/CallReflectedFunction.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Detail/CallReflectType.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Detail/FieldTraits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Detail/HasOperator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Detail/MakeTypeComparisonOperators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Detail/MakeTypeSpecialMembers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Field.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/GetStaticTypeInfo.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/GetTypeInfo.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/ReflectionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Reflector/FieldReflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Reflector/FunctionReflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Reflector/TypeReflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/StaticType/class.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/StaticType/class_to_dynamic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/StaticType/enum.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/StaticType/fundamental.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/StaticType/fundamental_predefined.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/Type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/TypeComparisonOperators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/TypeFlag.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/TypeRegistry.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/CppReflection/TypeSpecialMembers.hpp)
add_library(CppReflection INTERFACE ${module_source_files})
set_generic_compiler_options(CppReflection INTERFACE)
target_link_libraries(CppReflection INTERFACE edt)
target_include_directories(CppReflection INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/code/public)
target_include_directories(CppReflection INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/code/private)
