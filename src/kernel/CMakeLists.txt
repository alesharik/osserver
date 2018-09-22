function(load_profile ISA PLATFORM)
    # Obtain sources for the ISA
    file(GLOB ISA_SRCS "${ISA}/*.c" "${ISA}/*.s")
    file(GLOB PLATFORM_SRCS "${ISA}/${PLATFORM}/*.c" "${ISA}/${PLATFORM}/*.s")

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

file(GLOB INCLUDE_SRCS "include/*.c" "include/*.h")
include_directories(include)

load_profile("${TARGET_PROFILE}" "pc")

add_executable(osserver ${INCLUDE_SRCS} ${FIRST_SRCS} ${PLATFORM_SRCS} ${ISA_SRCS} ${GENERIC_SRCS})

set(CMAKE_ASM-ATT_COMPILE_OBJECT "<CMAKE_ASM-ATT_COMPILER> ${ISA_ASM_FLAGS} ${PLATFORM_ASM_FLAGS} -o <OBJECT> <SOURCE>")
set(CMAKE_C_FLAGS "${CMP_FLAGS} ${ISA_C_FLAGS} ${PLATFORM_C_FLAGS}" CACHE STRING "" FORCE)
set_target_properties(osserver PROPERTIES LINK_FLAGS "-T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")
add_custom_command(TARGET osserver POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy osserver ${PROJECT_SOURCE_DIR}/out/osserver.bin
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )