cmake_minimum_required(VERSION 3.22)

set(ModuleName "SpiInterface")
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
    ${PROJECT_NAME}Components
)

target_add_components(${ModuleName} ${ModuleName}Head)