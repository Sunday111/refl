cmake_minimum_required(VERSION 3.20)

project(CppReflection)

file(GLOB_RECURSE cppreflection_headers CONFIGURE_DEPENDS
    "modules/CppReflection/code/public/*.hpp")

add_library(CppReflection INTERFACE)
target_sources(CppReflection INTERFACE ${cppreflection_headers})
target_include_directories(CppReflection INTERFACE modules/CppReflection/code/public)
target_link_libraries(CppReflection INTERFACE edt)
target_compile_features(CppReflection INTERFACE cxx_std_20)

option(CPP_REFLECTION_WITH_TESTS "Enable CppReflection tests" OFF)
if(CPP_REFLECTION_WITH_TESTS)
    file(GLOB_RECURSE cppreflection_test_sources CONFIGURE_DEPENDS
        "modules/CppReflectionTests/code/private/*.cpp"
        "modules/CppReflectionTests/code/private/*.hpp")

    add_executable(CppReflectionTests ${cppreflection_test_sources})
    target_link_libraries(CppReflectionTests PRIVATE CppReflection gtest_main)
    target_compile_features(CppReflectionTests PRIVATE cxx_std_20)

    enable_testing()
    add_test(NAME CppReflectionTests COMMAND CppReflectionTests)
endif()
