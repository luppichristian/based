
# =========================================================================
# Dependencies
# =========================================================================

fetch_tracy()
fetch_sdl3()
fetch_efsw()
fetch_miniz()
fetch_olib()
fetch_libmath2()

# Define LM2_CUSTOM_ASSERT to use custom assert in libmath2.
target_compile_definitions(libmath2 PUBLIC LM2_CUSTOM_ASSERT)

# Suppress Clang's GNU extension folding warning without affecting other compilers.
set(
    BASED_CLANG_FOLDING_CONSTANT_WARNING
    $<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:-Wno-gnu-folding-constant>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>:-Wno-gnu-folding-constant>
)

# =========================================================================
# Library (always static for now)
# =========================================================================

# Source code
file(GLOB_RECURSE BASED_CORE_SOURCES "src/**.c")

# Default target (entry point is not handled)
add_library(based-core STATIC ${BASED_CORE_SOURCES})
target_include_directories(based-core PUBLIC include src)
target_link_libraries(based-core
    PRIVATE SDL3::SDL3 efsw miniz
    PUBLIC olib::static libmath2 Tracy::TracyClient
)
target_link_libraries(based-core PRIVATE $<$<PLATFORM_ID:Windows>:dbghelp>)
target_link_libraries(based-core PRIVATE $<$<PLATFORM_ID:Linux>:dl>)
target_compile_options(based-core PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})
foreach(source_file IN LISTS BASED_CORE_SOURCES)
    get_filename_component(source_dir "${source_file}" DIRECTORY)
    file(RELATIVE_PATH relative_dir "${CMAKE_CURRENT_SOURCE_DIR}/src" "${source_dir}")

    if(relative_dir STREQUAL "")
        set(unity_group "root")
    else()
        string(REPLACE "\\" "/" normd_dir "${relative_dir}")
        string(REGEX MATCH "^[^/]+" unity_group "${normd_dir}")
    endif()

    set_source_files_properties(${source_file} PROPERTIES UNITY_GROUP "${unity_group}")
endforeach()
set_target_properties(based-core PROPERTIES UNITY_BUILD ON UNITY_BUILD_MODE GROUP)

# =========================================================================
# Tests
# =========================================================================

if(BASED_BUILD_TESTS)
    enable_testing()
    fetch_googletest()
    include(GoogleTest)

    file(GLOB_RECURSE TEST_SOURCES "tests/**.cpp")
    add_executable(based-core-tests ${TEST_SOURCES})
    list(LENGTH TEST_SOURCES TEST_SOURCE_COUNT)
    if(TEST_SOURCE_COUNT GREATER 0)
        set_target_properties(
            based-core-tests
            PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE ${TEST_SOURCE_COUNT}
        )
    endif()
    target_link_libraries(based-core-tests PRIVATE based-core GTest::gtest)
    target_include_directories(based-core-tests PRIVATE tests)
    target_compile_options(based-core-tests PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})

    gtest_discover_tests(based-core-tests DISCOVERY_MODE PRE_TEST)
endif()
