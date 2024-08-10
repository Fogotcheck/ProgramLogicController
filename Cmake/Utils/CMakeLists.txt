cmake_minimum_required(VERSION 3.22)

project(Charger-mini VERSION 0.0.1 LANGUAGES C ASM)

include(Cmake/Utils/CustomOpts.cmake)
include(Cmake/Utils/ComponentsOpts.cmake)

add_executable(${PROJECT_NAME})

add_subdirectory(AppMain)
add_subdirectory(Lib)

target_post_build(${PROJECT_NAME})
target_install_binary(${PROJECT_NAME})

set(CPACK_PACKAGE_CHECKSUM MD5)
set(CPACK_PACKAGE_CHECKSUM MD5)
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
include(CPack)
