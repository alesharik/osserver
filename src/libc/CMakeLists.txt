project(libc C)

set(CMAKE_C_STANDARD 99)

file(GLOB SRC "include/*.c" "include/*.h" "include/**/*.c" "include/**/*.h")
add_library(libc ${SRC})
include_directories(include)
set_target_properties(libc PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(libc libkernel)