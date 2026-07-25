cmake_minimum_required(VERSION 3.20)
include(set_compiler_options)
set(module_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/ComparisonOperators.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/Fields.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/Methods.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/SpecialMembers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/StaticMethods.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/StaticTypeInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/TypeGUID.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/cross_tu_identity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/cross_tu_identity_a.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/cross_tu_identity_b.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/invocation_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/code/private/static_type_info_tests.cpp
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
