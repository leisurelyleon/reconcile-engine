# Centralized, reusable strict-warning configuration.
# Applied per-target via set_project_warnings(<target>).

function(set_project_warnings target)
    set(GCC_CLANG_WARNINGS
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wnull-dereference
        -Wdouble-promotion
        -Werror)

    set(MSVC_WARNINGS
        /W4
        /WX
        /permissive-)

    if(MSVC)
        target_compile_options(${target} PRIVATE ${MSVC_WARNINGS})
    else()
        target_compile_options(${target} PRIVATE ${GCC_CLANG_WARNINGS})
    endif()
endfunction()
