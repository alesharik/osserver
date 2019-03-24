function(load_profile ISA PLATFORM)
    # Obtain sources for the ISA
    file(GLOB ISA_SRCS "${ISA}/*.c" "${ISA}/*.s")
    file(GLOB PLATFORM_SRCS "${ISA}/${PLATFORM}/*.c" "${ISA}/${PLATFORM}/*.asm")

    # Load flags associated with ISA and Profile
    include("${ISA}/flags.cmake")
    include("${ISA}/${PLATFORM}/flags.cmake")

    # Now export our output variables
    set(ISA_SRCS ${ISA_SRCS} PARENT_SCOPE)
    set(PLATFORM_SRCS ${PLATFORM_SRCS} PARENT_SCOPE)
    set(PLATFORM_LAYOUT "${CMAKE_CURRENT_SOURCE_DIR}/${ISA}/${PLATFORM}/linker.ld" PARENT_SCOPE)

    # And specific flags
    set(ISA_C_FLAGS ${ISA_C_FLAGS} PARENT_SCOPE)
    set(ISA_ASM_FLAGS ${ISA_ASM_FLAGS} PARENT_SCOPE)
endfunction(load_profile)

file(GLOB INCLUDE_SRCS "include/**/*.c" "include/**/*.h" "include/*.c" "include/*.h" )

include_directories(include)

file(GLOB GENERIC_SRCS "generic/**/*.c" "generic/**/*.h" "generic/*.c" "generic/*.h" )

load_profile("${TARGET_PROFILE}" "pc")

project(kernel)
add_executable(kernel ${FIRST_SRCS} ${PLATFORM_SRCS} ${ISA_SRCS} ${GENERIC_SRCS} ${INCLUDE_SRCS})
add_library(libkernel SHARED ${FIRST_SRCS} ${PLATFORM_SRCS} ${ISA_SRCS} ${GENERIC_SRCS} ${INCLUDE_SRCS})

set(CMAKE_C_FLAGS "${CMP_FLAGS} ${ISA_C_FLAGS} ${PLATFORM_C_FLAGS}" CACHE STRING "" FORCE)
set_target_properties(kernel PROPERTIES LINK_FLAGS "-T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")
set_target_properties(libkernel PROPERTIES LINK_FLAGS "-T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")

add_custom_command(TARGET kernel POST_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/build/build.sh ${CMAKE_BINARY_DIR}/out ${CMAKE_CURRENT_BINARY_DIR}/kernel ${PLATFORM_LAYOUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

message("Built ISO file with grub will be located in ${CMAKE_BINARY_DIR}/out")
message("Built bin file will be located in ${CMAKE_BINARY_DIR}/out")