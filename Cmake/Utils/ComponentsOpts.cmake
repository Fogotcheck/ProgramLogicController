cmake_minimum_required(VERSION 3.22)

add_library(${PROJECT_NAME}Components INTERFACE)

function(target_add_warnings COMPONENT)
    target_compile_options(${COMPONENT} PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wfatal-errors
        -Wdouble-promotion
        -Wstrict-prototypes
        -Wstrict-overflow
        -Werror-implicit-function-declaration
        -Wfloat-equal
        -Wundef
        -Wshadow
        -Wwrite-strings
        -Wsign-compare
        -Wmissing-format-attribute
        -Wunreachable-code
        -Wcast-align
        -Wcast-function-type
        -Wcast-qual
        -Wnull-dereference
        -Wuninitialized
        -Wunused
        -Wunused-function
        -Wreturn-type
        -Wredundant-decls
    )
endfunction(target_add_warnings COMPONENT)

function(target_add_components COMPONENT COMPONENT_HEADS)
    target_add_warnings(${COMPONENT})
    target_link_libraries(${PROJECT_NAME}Components INTERFACE ${COMPONENT})

    foreach(X IN LISTS ${COMPONENT_HEADS})
        target_include_directories(${PROJECT_NAME}Components SYSTEM INTERFACE ${X})
    endforeach(X IN LISTS COMPONENT_HEADS)
endfunction(target_add_components COMPONENT HEADS)