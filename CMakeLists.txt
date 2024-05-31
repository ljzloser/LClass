cmake_minimum_required(VERSION 3.16)

project("LClass")
if(WIN32)
    set(OUTPUTDIR "Win32")
else()
    set(OUTPUTDIR "Linux")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
else()
    set(BUILD_TYPE "Release")
endif()

set(OUTPUT_PATH ${PROJECT_SOURCE_DIR}/x64/build/../${OUTPUTDIR}-${BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH})
link_directories(${OUTPUT_PATH})

add_subdirectory(LCore)
add_subdirectory(LWidget)
add_subdirectory(Demo)
