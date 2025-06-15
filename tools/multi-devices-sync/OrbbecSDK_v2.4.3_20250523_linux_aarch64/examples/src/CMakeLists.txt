# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.5)

file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_LIST_DIR} "*")

# dependecies
find_package(OpenCV QUIET)
if(NOT ${OpenCV_FOUND})
    message(WARNING "OpenCV not found, some examples may not be built. Please install OpenCV or set OpenCV_DIR to the directory of your OpenCV installation.")
endif()

# utils
add_subdirectory(utils)

# cpp examples
add_subdirectory(3.advanced.multi_devices_sync)
add_subdirectory(3.advanced.multi_devices_sync_gmsltrigger)
add_subdirectory(0.basic.enumerate)

# InstallRequiredSystemLibraries
include(InstallRequiredSystemLibraries)
