set(CMAKE_SYSTEM_NAME osserver)
set(CMAKE_SYSTEM_PROCESSOR x86)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER i686-elf-gcc)

set(CMAKE_C_SIZEOF_DATA_PTR 4)

set(CMP_FLAGS "-O2 -g -ffreestanding -fbuiltin -Wall -Wextra -std=gnu99 -msse")
set(TARGET_PROFILE "i386")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-nostdlib -nostdinc")
set(CMAKE_ASM_NASM_FLAGS ${CMAKE_ASM_NASM_FLAGS} -felf32)