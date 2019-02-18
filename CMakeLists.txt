cmake_minimum_required(VERSION 3.12)
project(osserver C)

enable_language(ASM_NASM)
if(NOT CMAKE_ASM_NASM_COMPILER_LOADED)
    message(FATAL_ERROR "NASM compiler required!")
endif()

set(CMAKE_C_STANDARD 99)

add_subdirectory(src/kernel)
add_subdirectory(src/libc)