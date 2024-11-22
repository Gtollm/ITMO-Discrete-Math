include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        fuzzy_tests
        fuzzy_sets_test.cpp
        fuzzy_relations_test.cpp
)

target_link_libraries(
        fuzzy_tests
        GTest::gtest_main
)

target_include_directories(fuzzy_tests PUBLIC ${PROJECT_SOURCE_DIR})
include_directories(include)

include(GoogleTest)

gtest_discover_tests(fuzzy_tests)