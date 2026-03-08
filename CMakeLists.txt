# MIT License
# Copyright (c) 2026 Christian Luppi

# =========================================================================
# Setup
# =========================================================================

cmake_minimum_required(VERSION 3.19)
project(based C CXX)
include(FetchContent)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_tracy.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_sdl3.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_efsw.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_miniz.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_olib.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_libmath2.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_googletest.cmake")

# Ensure third-party subprojects default to honoring pre-set option variables.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BASED_BUILD_TESTS  "Build tests"                ON)
option(BASED_ENABLE_FETCHCONTENT "Enable FetchContent fallback for missing dependencies" ON)

# =========================================================================
# Tracy
# =========================================================================

fetch_tracy()

# =========================================================================
# SDL3
# =========================================================================

fetch_sdl3()

# =========================================================================
# efsw
# =========================================================================

fetch_efsw()

# =========================================================================
# miniz
# =========================================================================

fetch_miniz()

# =========================================================================
# Olib
# =========================================================================

fetch_olib()

# =========================================================================
# libmath2
# =========================================================================

fetch_libmath2()

# Define LM2_CUSTOM_ASSERT to use custom assert in libmath2.
target_compile_definitions(libmath2 PUBLIC LM2_CUSTOM_ASSERT)

# Suppress Clang's GNU extension folding warning without affecting other compilers.
set(
    BASED_CLANG_FOLDING_CONSTANT_WARNING
    $<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:-Wno-gnu-folding-constant>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>:-Wno-gnu-folding-constant>
)

# Binary-kind compile definitions by target type.
function(based_apply_binary_type_defines target_name)
    get_target_property(target_type ${target_name} TYPE)
    if(NOT target_type)
        message(FATAL_ERROR "based_apply_binary_type_defines: target '${target_name}' has no TYPE")
    endif()

    if(target_type STREQUAL "EXECUTABLE")
        target_compile_definitions(${target_name} PRIVATE BIN_RUNNABLE)
    elseif(target_type STREQUAL "SHARED_LIBRARY" OR target_type STREQUAL "MODULE_LIBRARY")
        target_compile_definitions(${target_name} PRIVATE BIN_DYNAMIC_LIB)
    elseif(target_type STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${target_name} PRIVATE BIN_STATIC_LIB)
    elseif(target_type STREQUAL "OBJECT_LIBRARY")
        target_compile_definitions(${target_name} PRIVATE BIN_OBJ_LIB)
    else()
        message(FATAL_ERROR "based_apply_binary_type_defines: unsupported target type '${target_type}' for target '${target_name}'")
    endif()
endfunction()

# =========================================================================
# Library (always static for now)
# =========================================================================

file(GLOB_RECURSE BASED_SOURCES "src/**.c")
add_library(based STATIC ${BASED_SOURCES})
target_include_directories(based PUBLIC include)
target_link_libraries(based PRIVATE SDL3::SDL3 efsw miniz PUBLIC olib::static libmath2 Tracy::TracyClient)
target_compile_definitions(based PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(based PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})
based_apply_binary_type_defines(based)
target_compile_definitions(
    based
    INTERFACE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:BIN_RUNNABLE>
        $<$<OR:$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>,$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>>:BIN_DYNAMIC_LIB>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:BIN_STATIC_LIB>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,OBJECT_LIBRARY>:BIN_OBJ_LIB>
)

# =========================================================================
# Tests
# =========================================================================

if(BASED_BUILD_TESTS)
    enable_testing()

    # Ignore a placeholder test registered by libmath2 when its test binary
    # is intentionally disabled in this workspace configuration.
    file(WRITE "${CMAKE_BINARY_DIR}/CTestCustom.cmake" "set(CTEST_CUSTOM_TESTS_IGNORE libmath2-tests_NOT_BUILT)\n")

    fetch_googletest()

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/**.cpp")
    add_executable(based-tests ${TEST_SOURCES})
    target_link_libraries(based-tests PRIVATE based GTest::gtest)
    target_include_directories(based-tests PRIVATE tests)
    target_compile_options(based-tests PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})
    based_apply_binary_type_defines(based-tests)

    include(GoogleTest)
    gtest_discover_tests(based-tests PROPERTIES RUN_SERIAL TRUE)

    # libmath2 may register a gtest_discover placeholder when its test binary
    # is not built in this workspace configuration; ignore that non-project test.
    if(TEST libmath2-tests_NOT_BUILT)
        set_tests_properties(libmath2-tests_NOT_BUILT PROPERTIES DISABLED TRUE)
    endif()
endif()
