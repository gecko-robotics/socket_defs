# GTest -----------------
# include(FetchContent)
# FetchContent_Declare(gtest
# #   GIT_REPOSITORY "https://github.com/google/googletest"
# #   GIT_TAG "origin/main"
# #   SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/gtest"
#     URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
# )
# set(EXAMPLES OFF CACHE INTERNAL "Dont build examples")
# FetchContent_MakeAvailable(gtest)
# if(gtest_POPULATED)
#     message(STATUS "=> Found gtest")
# else()
#     message(STATUS "*** Didn't find gtest")
# endif()


find_package(GTest REQUIRED)
if(GTest_FOUND)
    message(STATUS "=> Found gtest")
else()
    message(STATUS "*** Didn't find gtest")
endif()

enable_testing()

LIST(APPEND gtests
    test_socket
)

add_executable(test-all ${gtests})
target_link_libraries(test-all GTest::gtest_main ${PROJECT_NAME})
add_test(NAME test-all COMMAND test-all)
include(GoogleTest)
gtest_discover_tests(test-all)