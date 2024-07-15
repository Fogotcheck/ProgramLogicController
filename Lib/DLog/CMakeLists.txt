cmake_minimum_required(VERSION 3.22)

set(ModuleName "DLog")
set(${ModuleName}Src Src/${ModuleName}.c)
set(${ModuleName}Head Inc)

add_library(${ModuleName} ${${ModuleName}Src})
target_include_directories(
    ${ModuleName}
    PRIVATE
    ${${ModuleName}Head}
)

target_link_libraries(
    ${ModuleName}
    ${PROJECT_NAME}StInterface
    FreeRtos
)

target_compile_definitions(
    ${ModuleName}
    PRIVATE
    "DLOG_SET_DEFAULT_LEVEL=0x1c"
)

target_add_components(${ModuleName} ${ModuleName}Head)