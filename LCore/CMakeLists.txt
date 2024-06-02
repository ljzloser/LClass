cmake_minimum_required(VERSION 3.16)
project(LCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
)
qt_standard_project_setup()

include_directories(${PROJECT_SOURCE_DIR}/include)
FILE(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE_FILES)

add_library(LCore SHARED 
    ${PROJECT_SOURCES}    
    ${SOURCE_FILES}
    ${HEADER_FILES}
)
target_link_libraries(LCore
    PRIVATE
        Qt::Core
)

target_compile_definitions(LCore
    PRIVATE
        LCORE_LIB
)
