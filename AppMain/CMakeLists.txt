cmake_minimum_required(VERSION 3.22)

set(ModuleName "AppMain")
set(${ModuleName}Src Src/${ModuleName}.c)
set(${ModuleName}Head Inc)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${${ModuleName}Src}
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${${ModuleName}Head}
)

target_link_libraries(
    ${PROJECT_NAME}
    ${PROJECT_NAME}StInterface
    FreeRtos
)