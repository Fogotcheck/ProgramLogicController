cmake_minimum_required(VERSION 3.22)

set(ExeProvider_env "\${command:cmake.launchTargetPath}")
set(workspaceFolder_env "\${workspaceFolder}")

set(JSON_CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/Cmake/Utils/CustomConfig/VScodeJsonIni")

if(NOT CMAKE_TOOLCHAIN_FILE)
    find_file(CMAKE_TOOLCHAIN_FILE
        toolchain_arm_cortex_m4.cmake
        ${CMAKE_SOURCE_DIR}/Cmake/Toolchain
    )
endif()

configure_file("${JSON_CONFIG_FILE_PATH}/launch_arm.in"
    "${CMAKE_SOURCE_DIR}/build/VScodeJson/launch.json")

configure_file("${JSON_CONFIG_FILE_PATH}/c_cpp_properties.in"
    "${CMAKE_SOURCE_DIR}/build/VScodeJson/c_cpp_properties.json")
configure_file("${JSON_CONFIG_FILE_PATH}/extensions.in"
    "${CMAKE_SOURCE_DIR}/build/VScodeJson/extensions.json")
configure_file("${JSON_CONFIG_FILE_PATH}/settings.in"
    "${CMAKE_SOURCE_DIR}/build/VScodeJson/settings.json")

add_custom_target(MakeJsonVScode
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/VScodeJson/c_cpp_properties.json
    ${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/VScodeJson/launch.json
    ${CMAKE_SOURCE_DIR}/.vscode/launch.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/VScodeJson/extensions.json
    ${CMAKE_SOURCE_DIR}/.vscode/extensions.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/VScodeJson/settings.json
    ${CMAKE_SOURCE_DIR}/.vscode/settings.json
    COMMENT "Moving .json files to .vscode folder"
    VERBATIM
)

