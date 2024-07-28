cmake_minimum_required(VERSION 3.22)

set(ModuleName "ActuatMech")
set(${ModuleName}Head 
    Types
    Events
    Def
)

add_library(${ModuleName} INTERFACE)
target_include_directories(
    ${ModuleName}
    SYSTEM INTERFACE
    ${${ModuleName}Head}
)
