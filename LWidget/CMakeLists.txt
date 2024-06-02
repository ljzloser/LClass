cmake_minimum_required(VERSION 3.16)
project(LWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
)
qt_standard_project_setup()
set(CMAKE_AUTORCC ON)
include_directories(${PROJECT_SOURCE_DIR}/include)
FILE(GLOB_RECURSE RC_FILES ${PROJECT_SOURCE_DIR}/resource/*.rc)
FILE(GLOB_RECURSE QRC_FILES ${PROJECT_SOURCE_DIR}/resource/*.qrc)
FILE(GLOB_RECURSE UI_FILES ${PROJECT_SOURCE_DIR}/ui/*.ui)
FILE(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE_FILES)
include_directories(${PROJECT_SOURCE_DIR}/../LCore/include)


add_library(LWidget SHARED 
        ${SOURCE_FILES}
        ${UI_FILES}
        ${RC_FILES}
        ${QRC_FILES}
        ${HEADER_FILES}
)

target_link_libraries(LWidget
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Gui
    LCore
)

target_compile_definitions(LWidget
    PRIVATE
        LWIDGET_LIB
)
