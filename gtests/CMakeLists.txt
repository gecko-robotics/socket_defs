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

message(STATUS "=> socket_defs gtest build")

FetchContent_MakeAvailable(gtest)

enable_testing()

LIST(APPEND gtests
    test_socket
    # test_marko
)

foreach(MAIN ${gtests})
    add_executable(${MAIN} ${MAIN}.cpp)
    target_link_libraries(${MAIN} PRIVATE
        ${PROJECT_NAME}
        GTest::gtest_main)
endforeach()

include(GoogleTest)
# gtest_discover_tests(test_marko)