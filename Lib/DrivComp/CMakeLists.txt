cmake_minimum_required(VERSION 3.22)

set(ModuleName "DrivComp")
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
    MainEvents
    ${PROJECT_NAME}Components
)

add_subdirectory(StInterface)
add_subdirectory(Drivers)

target_add_components(${ModuleName} ${ModuleName}Head)