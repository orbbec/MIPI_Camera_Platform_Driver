# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

add_custom_target(obtool_orbbec_sdk ALL)

macro(add_open_orbbec_sdk)
    
    set(ORBBEC_SDK_ROOT ${SRC_3RDPARTY_DIR}/OpenOrbbecSDK/${TOOL_PLATFORM})
    set(OrbbecSDK_DIR ${ORBBEC_SDK_ROOT}/lib)
    find_package(OrbbecSDK REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC ob::OrbbecSDK)

    if(WIN32)
        file(GLOB OrbbecSDKFiles ${ORBBEC_SDK_ROOT}/bin/*)
        list(FILTER OrbbecSDKFiles INCLUDE REGEX ".*.dll$")
        install(FILES ${OrbbecSDKFiles} DESTINATION . COMPONENT dev)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${OrbbecSDKFiles}  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        )

        install(FILES ${ORBBEC_SDK_ROOT}/bin/OrbbecSDKConfig.md DESTINATION . COMPONENT dev)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ORBBEC_SDK_ROOT}/bin/OrbbecSDKConfig.md  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        )
    
        install(FILES ${ORBBEC_SDK_ROOT}/bin/OrbbecSDKConfig.xml DESTINATION . COMPONENT dev)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ORBBEC_SDK_ROOT}/bin/OrbbecSDKConfig.xml  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        )
    
        # extensions
        set(ORBBEC_SDK_EXTENSION_DIR ${ORBBEC_SDK_ROOT}/bin/extensions)
        install(DIRECTORY "${ORBBEC_SDK_EXTENSION_DIR}/" DESTINATION extensions)
        file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${ORBBEC_SDK_EXTENSION_DIR}/  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/
        )
    else(UNIX)
        file(GLOB OrbbecSDKFiles ${ORBBEC_SDK_ROOT}/lib/*.so*)
        install(FILES ${OrbbecSDKFiles} DESTINATION . COMPONENT dev)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${OrbbecSDKFiles}  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        )

        install(FILES ${ORBBEC_SDK_ROOT}/lib/OrbbecSDKConfig.md DESTINATION . COMPONENT dev)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ORBBEC_SDK_ROOT}/lib/OrbbecSDKConfig.md  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        )
    
        install(FILES ${ORBBEC_SDK_ROOT}/lib/OrbbecSDKConfig.xml DESTINATION . COMPONENT dev)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ORBBEC_SDK_ROOT}/lib/OrbbecSDKConfig.xml  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        )
            
        set(ORBBEC_SDK_EXTENSION_DIR ${ORBBEC_SDK_ROOT}/lib/extensions)
        # extensions
        install(DIRECTORY "${ORBBEC_SDK_EXTENSION_DIR}/" DESTINATION extensions)
        file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/)
        add_custom_command(
            TARGET obtool_orbbec_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${ORBBEC_SDK_EXTENSION_DIR}/  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/
        )
    endif()

endmacro()
