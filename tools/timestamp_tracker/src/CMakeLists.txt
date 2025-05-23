# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

add_executable(${PROJECT_NAME})

# OpenOrbbecSDK
include(orbbec_sdk)
add_open_orbbec_sdk()
# dependencies of 3rdparty
add_dependencies(${PROJECT_NAME} obtool_orbbec_sdk)

file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# utils
add_subdirectory(utils)

target_link_libraries(${PROJECT_NAME} PRIVATE ob::examples::utils)

# InstallRequiredSystemLibraries
include(InstallRequiredSystemLibraries)
