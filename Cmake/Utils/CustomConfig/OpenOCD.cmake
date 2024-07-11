cmake_minimum_required(VERSION 3.22)

find_program(OPENOCD_EXE openocd)

get_filename_component(OPENOCD_PATH "${OPENOCD_EXE}" DIRECTORY)

set(OPENOCD_TARGET "target/stm32f4x.cfg")

set(OPENOCD_INTERFACE "interface/stlink.cfg")

add_custom_target(OpenOCDflash
    COMMAND ${OPENOCD_EXE} -f "${OPENOCD_INTERFACE}" -f "${OPENOCD_TARGET}" -c "program ${PROJECT_NAME} verify reset exit"
    DEPENDS ${PROJECT_NAME}
    COMMENT "Flashing ${PROJECT_NAME}.elf with OpenOCD"
)
