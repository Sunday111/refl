cmake_minimum_required(VERSION 3.29)
include(set_compiler_options)
set(module_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/call_reflected_function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/detail/call_reflect_type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/detail/field_traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/detail/has_operator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/detail/make_type_comparison_operators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/detail/make_type_special_members.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/field.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/get_static_type_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/get_type_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/reflection_provider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/reflector/field_reflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/reflector/function_reflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/reflector/type_reflector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/static_type/class.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/static_type/class_to_dynamic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/static_type/enum.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/static_type/fundamental.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/static_type/fundamental_predefined.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/type_comparison_operators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/type_flag.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/type_registry.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/public/refl/type_special_members.hpp)
add_library(refl INTERFACE ${module_source_files})
set_generic_compiler_options(refl INTERFACE)
target_link_libraries(refl INTERFACE edt)
target_include_directories(refl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/code/public)
target_include_directories(refl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/code/private)
