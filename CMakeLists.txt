# MIT License
# Copyright (c) 2026 Christian Luppi

cmake_minimum_required(VERSION 3.19)
project(based C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BASED_BUILD_TESTS  "Build tests"                ON)
option(BASED_GTEST_FETCH  "Fetch GoogleTest if not found" ON)

# ── Library ──────────────────────────────────────────────────────────────────

file(GLOB BASED_SOURCES "src/*.c")
add_library(based STATIC ${BASED_SOURCES})
target_include_directories(based PUBLIC include)

# ── Tests ─────────────────────────────────────────────────────────────────────

if(BASED_BUILD_TESTS)
    find_package(GTest QUIET)

    if(NOT GTest_FOUND AND BASED_GTEST_FETCH)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
        target_compile_options(gtest      PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-undef>)
        target_compile_options(gtest_main PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-undef>)
    endif()

    file(GLOB TEST_SOURCES "tests/*.cpp")
    add_executable(based-tests ${TEST_SOURCES})
    target_link_libraries(based-tests PRIVATE based GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(based-tests)
endif()
