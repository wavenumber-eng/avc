#
# Boiler Plate
#
# This is helpful to set up the project if we are configuration the project standalone
# without the other test projects.
#
message(STATUS "Cmake System name is ${CMAKE_SYSTEM_NAME}")


if(DEFINED SDL2_DIR)
    message(STATUS "SDL2 was already defined as ${SDL2_DIR}")
else()
    
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Configuring SDL for Windows")
        set(SDL2_DIR  "${CMAKE_CURRENT_LIST_DIR}/sdl2/cmake")
	message(STATUS "SDL2 location is ${SDL2_DIR}")

    else()
        message(STATUS "Configuring SDL for Linux")
    endif()

endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
message(STATUS "SDL: Using ${SDL2_DIR}")

if(DEFINED EGFX_LOC)
    message(STATUS "eGFX location is ${EGFX_LOC}")
else()
    set(EGFX_LOC   "${CMAKE_CURRENT_LIST_DIR}/../../../../src")
    message(STATUS "Usng default eGFX location : ${EGFX_LOC}")
endif()

if(DEFINED TARGET)
     message(STATUS "Target name is ${TARGET}")
else()
     set(TARGET       ${PROJECT_NAME})
     message(STATUS "Using default target name from project : ${TARGET}")
endif()

