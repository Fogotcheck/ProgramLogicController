cmake_minimum_required(VERSION 3.22)

set(CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set(CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")

# Specify toolchain. NOTE When building from inside STM32CubeIDE the location of the toolchain is resolved by the "MCU Toolchain" project setting (via PATH).
set(TOOLCHAIN_PREFIX    "arm-none-eabi-")
set(CMAKE_C_COMPILER    "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_ASM_COMPILER  "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER  "${TOOLCHAIN_PREFIX}g++")
set(CMAKE_AR            "${TOOLCHAIN_PREFIX}ar")
set(CMAKE_LINKER        "{TOOLCHAIN_PREFIX}ld")
set(CMAKE_OBJCOPY       "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_RANLIB        "${TOOLCHAIN_PREFIX}ranlib")
set(CMAKE_SIZE          "${TOOLCHAIN_PREFIX}size")
set(CMAKE_STRIP         "${TOOLCHAIN_PREFIX}ld")
set(CMAKE_GDB           "${TOOLCHAIN_PREFIX}gdb")

set(PROJECT_TYPE_EXECUTABLE       "exe")
set(PROJECT_TYPE_STATIC_LIBRARY   "static-lib")
set(PROJECT_TYPE                  "exe")
set(CMAKE_EXECUTABLE_SUFFIX       ".elf")

add_compile_options(
  -fdata-sections
  -ffunction-sections
  -Wall
)

add_link_options(
  --specs=nosys.specs
  -Wl,--gc-sections
  -static
  -Wl,--start-group
  -lc
  -lm
  -Wl,--end-group
)