cmake_minimum_required(VERSION 3.22)

include(${CMAKE_SOURCE_DIR}/Cmake/Utils/BuildOpts.cmake)

set(MCPU "-mcpu=cortex-m4" CACHE STRING "" FORCE)
set(MFPU "-mfpu=fpv4-sp-d16" CACHE STRING "" FORCE)
set(MFLOAT_ABI "-mfloat-abi=hard" CACHE STRING "" FORCE)
set(MCU_DEVICE "STM32F439xx" CACHE STRING "" FORCE)
set(MCU_LINCER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/STM32F439ZITX_FLASH.ld"
    CACHE STRING "" FORCE
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    Core/Src/main.c
    Core/Src/stm32f4xx_hal_msp.c
    Core/Src/stm32f4xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Startup/startup_stm32f439zitx.s
    Core/Src/system_stm32f4xx.c
    Core/Src/stm32f4xx_hal_timebase_tim.c
)

add_library(${PROJECT_NAME}StInterface INTERFACE)
target_add_CPU_flags(${PROJECT_NAME}StInterface)
target_include_directories(
    ${PROJECT_NAME}StInterface
    SYSTEM INTERFACE
    Core/Inc
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
    Drivers/CMSIS/Include
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
)

file(GLOB_RECURSE ${PROJECT_NAME}StDriversSrc
    Drivers/STM32F4xx_HAL_Driver/Src/*.c
)

add_library(${PROJECT_NAME}StDrivers
    ${${PROJECT_NAME}StDriversSrc}
)

target_link_libraries(
    ${PROJECT_NAME}StDrivers
    ${PROJECT_NAME}StInterface
)
target_link_libraries(
    ${PROJECT_NAME}StInterface
    INTERFACE
    ${PROJECT_NAME}StDrivers
)