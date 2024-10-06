cmake_minimum_required(VERSION 3.10)

project(egfx_test_projects)

set(EGFX_LOC      "${CMAKE_CURRENT_LIST_DIR}/../../src")

set(TARGETS test_64_32_32BPP test_451_272_8BPP_XRGB222 test_256_64_4BPP)


if(DEFINED SDL2_DIR)
    message(STATUS "SDL2 location is ${SDL2_DIR}")
else()
    
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Configuring SDL for Windows")
        set(SDL2_DIR  "${CMAKE_CURRENT_LIST_DIR}/sdl2/cmake")
    else()
        message(STATUS "Configuring SDL for Linux builds")
    endif()
endif()


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
message(STATUS "SDL: Using ${SDL2_DIR}")


foreach(TARGET_NAME IN LISTS TARGETS) 
    set(TARGET ${TARGET_NAME})
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/prj/${TARGET_NAME}/" ${TARGET_NAME})
endforeach()
