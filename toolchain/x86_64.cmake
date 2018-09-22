set(CMAKE_SYSTEM_NAME Kiznix)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER x86_64-elf-gcc)

set(CMAKE_C_SIZEOF_DATA_PTR 8)

set(CMP_FLAGS "-O2 -g -ffreestanding -fbuiltin -Wall -Wextra -std=gnu99 -mcmodel=kernel -mno-red-zone")