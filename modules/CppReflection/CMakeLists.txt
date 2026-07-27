cmake_minimum_required(VERSION 3.20)
include(set_compiler_options)
set(module_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/call_reflected_function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/detail/call_reflect_type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/detail/field_traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/detail/has_operator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/detail/make_type_comparison_operators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/detail/make_type_special_members.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/field.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/get_static_type_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/get_type_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/reflection_provider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/reflector/field_reflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/reflector/function_reflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/reflector/type_reflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/static_type/class.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/static_type/class_to_dynamic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/static_type/enum.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/static_type/fundamental.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/static_type/fundamental_predefined.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/type_comparison_operators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/type_flag.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/type_registry.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/cpp_reflection/type_special_members.hpp)
add_library(CppReflection INTERFACE ${module_source_files})
set_generic_compiler_options(CppReflection INTERFACE)
target_link_libraries(CppReflection INTERFACE edt)
target_include_directories(CppReflection INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/code/public)
target_include_directories(CppReflection INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/code/private)
