cmake_minimum_required(VERSION 3.22)

option(OPENOCD_ENABLE "Custom target openocd flash" OFF)
option(ENVIRONMENT_VSCODE "Custom target make json for VSCode" OFF)


if(OPENOCD_ENABLE)
    include(${CMAKE_SOURCE_DIR}/Cmake/Utils/CustomConfig/OpenOCD.cmake)
endif()
if(ENVIRONMENT_VSCODE)
    include(${CMAKE_SOURCE_DIR}/Cmake/Utils/CustomConfig/VsCode.cmake)
endif()


find_program(ELF2DFUSE elf2dfuse)

function(target_post_build TargetName)
    set(AllTargets "${AllTargets}" CACHE INTERNAL "My list of items" FORCE)
    add_custom_command(
        TARGET ${TargetName} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${TargetName} ${CMAKE_BINARY_DIR}/${TargetName}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${TargetName} ${CMAKE_BINARY_DIR}/${TargetName}.hex
        COMMAND ${CMAKE_OBJDUMP} -S ${TargetName} > ${CMAKE_BINARY_DIR}/${TargetName}.S
        COMMAND ${CMAKE_NM} -a -l -S -s ${TargetName} > ${CMAKE_BINARY_DIR}/${TargetName}.map
    )
    install(TARGETS ${TargetName} DESTINATION bin)
    install(FILES ${CMAKE_BINARY_DIR}/${TargetName}.bin DESTINATION bin)
    install(FILES ${CMAKE_BINARY_DIR}/${TargetName}.hex DESTINATION bin)

    if(ELF2DFUSE)
        add_custom_command(
            TARGET ${TargetName} POST_BUILD
            COMMAND ${ELF2DFUSE} ${TargetName} ${CMAKE_BINARY_DIR}/${TargetName}.dfu
        )
        install(FILES ${CMAKE_BINARY_DIR}/${TargetName}.dfu DESTINATION bin)
    else()
        message(WARNING
            "dfu file will not be generated because elf2dfu1se doesn't exist")
    endif()

    add_custom_command(
        TARGET ${TargetName} POST_BUILD
        COMMAND ${CMAKE_SIZE} -A ${TargetName}
        COMMAND ${CMAKE_NM} -A -S -t d -s --size-sort --print-size ${TargetName} > FuctionSizeInfo.map
        COMMAND echo "Build - success"
        COMMAND echo "CMAKE_BUILD_TYPE::${CMAKE_BUILD_TYPE}"
    )
endfunction(target_post_build TargetName)

function(target_install_binary TargetName)
    add_custom_target(Install${TargetName}
        COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${CMAKE_BINARY_DIR}
        DEPENDS ${AllTargets}
        COMMENT "Installing ${TargetName}"
    )
endfunction(target_install_binary TargetName)

