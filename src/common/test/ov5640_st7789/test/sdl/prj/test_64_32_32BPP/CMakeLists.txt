cmake_minimum_required(VERSION 3.10)

project(egfx_sdl_test_64_32_32BPP)

message(STATUS "Cmake System name is ${CMAKE_SYSTEM_NAME}")


if(DEFINED SDL2_DIR)
    message(STATUS "SDL2 was already defined as ${SDL2_DIR}")
else()
    
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        message(STATUS "Configuring SDL for Windows")
        set(SDL2_DIR  "${CMAKE_CURRENT_LIST_DIR}/../../sdl2/cmake")
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
    message(STATUS "Uisng default eGFX location : ${EGFX_LOC}")
endif()

if(DEFINED TARGET)
     message(STATUS "Target name is ${TARGET}")
else()
     set(TARGET       ${PROJECT_NAME})
     message(STATUS "Using default target name from project : ${TARGET}")
endif()


#options for the SDL Driver
set(SDL_SIM_IMAGE_PLANE_TYPE "eGFX_IMAGE_PLANE_32BPP_XRGB888")

set(SDL_INITIAL_ZOOM           1)

#if enabled, the driver will draw larger pixels with some space in between
#to simulate chunk LED screens and LCD screens
set(SDL_SIM_GRID_ENABLE        1)
set(SDL_SIM_GRID_PIXEL_BORDER  1)
set(SDL_SIM_GRID_PIXEL_SIZE    4)
set(SDL_SIM_GRID_BACKGROUND_COLOR 0x202020)
set(SDL_SIM_PHYSICAL_SCREEN_SIZE_X 64)
set(SDL_SIM_PHYSICAL_SCREEN_SIZE_Y 32)


configure_file(${EGFX_LOC}/IO/Display/SDL/eGFX_DisplayDriver_SDL.h.in
               ${CMAKE_CURRENT_LIST_DIR}/src/eGFX_DisplayDriver_SDL.h
               )

set(TEST_SRC_FOLDER "${CMAKE_CURRENT_LIST_DIR}/src")

set(TEST_SRC    "${TEST_SRC_FOLDER}/eGFX_Test.c"
                "${TEST_SRC_FOLDER}/Sprites_32BPP_XRGB888.c" 
                "${EGFX_LOC}/IO/Display/SDL/eGFX_DisplayDriver_SDL.c"
                 )

add_executable(${TARGET}  ${TEST_SRC})

target_compile_definitions(${TARGET} PUBLIC  SIMULATOR=1 )

#target_compile_options(${TARGET} PUBLIC)

set(EGFX_CONFIG_LOC "${CMAKE_CURRENT_LIST_DIR}/src")

set(ASSET_LOC "${CMAKE_CURRENT_LIST_DIR}/assets")

set(TOOLS_LOC "${CMAKE_CURRENT_LIST_DIR}/../../../../tools")

target_include_directories(${TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src"
                           ) 
                                                                    
add_subdirectory(${EGFX_LOC} egfx)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    execute_process(COMMAND "${TOOLS_LOC}/eGFX_Tools.exe" "GenerateSprites" "${ASSET_LOC}" "eGFX_IMAGE_PLANE_32BPP_XRGB888" "${TEST_SRC_FOLDER}"
                RESULT_VARIABLE ASSET_GEN_OUTPUT 
                )
else()

    message(STATUS "Using mono for running the tools on Linux, etc.")

    execute_process(COMMAND "mono" "${TOOLS_LOC}/eGFX_Tools.exe" "GenerateSprites" "${ASSET_LOC}" "eGFX_IMAGE_PLANE_32BPP_XRGB888" "${TEST_SRC_FOLDER}"
                RESULT_VARIABLE ASSET_GEN_OUTPUT 
                )
endif()

message("${ASSET_GEN_OUTPUT}")

target_link_libraries(${TARGET} ${SDL2_LIBRARIES})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

    #needed for visual studio outputs.  Need to make sure the SDL dll's exist next to the binary
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SDL2_DIR}/../lib/x64"
            $<TARGET_FILE_DIR:${TARGET}>)
endif()