cmake_minimum_required(VERSION 3.22)

function(target_add_CPU_flags TargetName)
    target_compile_options(
        ${TargetName}
        INTERFACE
        ${MCPU}
        ${MFPU}
        ${MFLOAT_ABI}
        -T${MCU_LINCER_SCRIPT}
        -std=gnu11
    )

    target_link_options(
        ${TargetName}
        INTERFACE
        ${MCPU}
        ${MFPU}
        ${MFLOAT_ABI}
        -T${MCU_LINCER_SCRIPT}
    )

    target_compile_definitions(
        ${TargetName}
        INTERFACE
        ${MCU_DEVICE}
        ${MCPU_CORE}
        "USE_HAL_DRIVER"
        "__NEWLIB__"
    )  
endfunction(target_add_CPU_flags TargetName)
