cmake_minimum_required(VERSION 3.22)

set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)

add_library(freertos_config INTERFACE)

target_include_directories(
    freertos_config
    SYSTEM INTERFACE
    FreeRtosConf
    FreeRtosCust
)

target_link_libraries(
    freertos_config
    INTERFACE
    ${PROJECT_NAME}StInterface
)

add_subdirectory(FreeRTOS/Source)

add_library(
    FreeRtos
    FreeRtosCust/FreeRtosCustom.c
)

target_link_libraries(
    FreeRtos
    freertos_kernel
    freertos_config
)

target_link_libraries(
    freertos_kernel
    INTERFACE
    FreeRtos
)