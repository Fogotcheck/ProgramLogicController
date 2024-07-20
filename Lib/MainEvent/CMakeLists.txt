cmake_minimum_required(VERSION 3.22)

set(ModuleName "MainEvents")
set(${ModuleName}Head Inc)

add_library(${ModuleName} INTERFACE)
target_include_directories(
    ${ModuleName}
    SYSTEM INTERFACE
    ${${ModuleName}Head}
)

target_link_libraries(
    ${ModuleName}
    INTERFACE
    ${PROJECT_NAME}StInterface
    FreeRtos
)
