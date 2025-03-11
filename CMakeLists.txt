cmake_minimum_required(VERSION 3.22.0)
project(socket_defs VERSION 2025.03.10 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
    cmake_host_system_information(RESULT HOST QUERY HOSTNAME)
    cmake_host_system_information(RESULT OSN QUERY OS_NAME)
    cmake_host_system_information(RESULT OS_VERSION QUERY OS_RELEASE)
    cmake_host_system_information(RESULT PROC QUERY PROCESSOR_DESCRIPTION)

    message(STATUS "-------------------------------------")
    message(STATUS "  Project: ${PROJECT_NAME}")
    message(STATUS "  Version: ${CMAKE_PROJECT_VERSION}")
    message(STATUS "  C++ ${CMAKE_CXX_STANDARD}")
    message(STATUS "-------------------------------------")
    message(STATUS " ${HOST}")
    message(STATUS " ${OSN}: ${OS_VERSION}")
    message(STATUS " ${PROC}")
    message(STATUS "-------------------------------------")

    set(BUILD_EXAMPLES ON)
    set(BUILD_TESTS ON)
else()
    message(STATUS "-> ${PROJECT_NAME} is submodule")
    set(BUILD_EXAMPLES OFF)
    set(BUILD_GTESTS OFF)
endif()

message(STATUS "->> ${${PROJECT_NAME}_SOURCE_DIR}")
message(STATUS "->> ${CMAKE_CURRENT_SOURCE_DIR}")

# Library =====================================================================
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
    INTERFACE src/
    # $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    # $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Examples ====================
# message(STATUS "Building ${PROJECT_NAME} examples is ${BUILD_EXAMPLES}")
# if (BUILD_EXAMPLES)
#     add_subdirectory(examples)
# endif()

# Tests =======================
message(STATUS "Building ${PROJECT_NAME} gtests is ${BUILD_GTESTS}")
if (BUILD_TESTS)
    add_subdirectory(gtests)
endif()
