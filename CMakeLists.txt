cmake_minimum_required(VERSION 3.19)

project(psForm)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -fsanitize=address,undefined -g -O0")

set(src ${CMAKE_SOURCE_DIR}/src/main.c
        ${CMAKE_SOURCE_DIR}/src/lex_analizer.c
        ${CMAKE_SOURCE_DIR}/src/ps_form.c
        ${CMAKE_SOURCE_DIR}/src/ps_form_operators.c
        )

set(exec psForm.out)
add_executable(${exec}
    ${src}
    )
target_include_directories(${exec} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(${exec} PRIVATE
    $<$<CONFIG:Debug>:DEBUG=1>
)

include(CTest)
enable_testing()

file(GLOB testfiles "${CMAKE_SOURCE_DIR}/tests/*.dat")
foreach(file ${testfiles})
    add_test (NAME ${file}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test.py ${CMAKE_SOURCE_DIR} ${file})
    set_tests_properties(${file} PROPERTIES DEPENDS ${exec})
endforeach()
