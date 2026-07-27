cmake_minimum_required(VERSION 3.20)
include(set_compiler_options)
set(module_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/comparison_operators.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/cross_tu_identity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/cross_tu_identity_a.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/cross_tu_identity_b.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/fields.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/invocation_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/methods.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/special_members.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/static_methods.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/static_type_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/static_type_info_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/type_guid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/type_hierarchy_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/type_registry_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/util.cpp)
add_executable(CppReflectionTests ${module_source_files})
set_generic_compiler_options(CppReflectionTests PRIVATE)
target_link_libraries(CppReflectionTests PRIVATE CppReflection
                                                 gtest_main)
target_include_directories(CppReflectionTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/code/public)
target_include_directories(CppReflectionTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/code/private)
enable_testing()
include(GoogleTest)
gtest_discover_tests(CppReflectionTests)
