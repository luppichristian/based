
# =========================================================================
# Library (always static for now)
# =========================================================================

# Source code
file(GLOB_RECURSE BASED_GFX_SOURCES "src/**.c")

add_library(based-gfx STATIC ${BASED_GFX_SOURCES})
target_include_directories(based-gfx PUBLIC include src)
target_link_libraries(based-gfx PUBLIC based-core)

# =========================================================================
# Tests
# =========================================================================

if(BASED_BUILD_TESTS)
    enable_testing()
    fetch_googletest()
    include(GoogleTest)

    file(GLOB_RECURSE TEST_SOURCES "tests/**.cpp")
    add_executable(based-gfx-tests ${TEST_SOURCES})
    list(LENGTH TEST_SOURCES TEST_SOURCE_COUNT)
    if(TEST_SOURCE_COUNT GREATER 0)
        set_target_properties(
            based-gfx-tests
            PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE ${TEST_SOURCE_COUNT}
        )
    endif()
    target_link_libraries(based-gfx-tests PRIVATE based-gfx GTest::gtest)
    target_include_directories(based-gfx-tests PRIVATE tests)
    target_compile_definitions(based-gfx-tests PRIVATE ENTRY_WINDOWS_CONSOLE)
    gtest_discover_tests(based-gfx-tests DISCOVERY_MODE PRE_TEST)
endif()
