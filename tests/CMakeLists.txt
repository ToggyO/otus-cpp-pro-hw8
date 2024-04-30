set(TEST_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

configure_file(config.h.in config.h @ONLY)

find_package(GTest REQUIRED)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PUBLIC
    GTest::gtest
    GTest::gtest_main
    ${LIB_BINARY}
)

include(GoogleTest)
gtest_discover_tests(tests)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})