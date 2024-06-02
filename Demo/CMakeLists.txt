cmake_minimum_required(VERSION 3.16)
project(Demo LANGUAGES CXX)

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
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ui)
include_directories(${PROJECT_SOURCE_DIR}/include)
FILE(GLOB_RECURSE RC_FILES ${PROJECT_SOURCE_DIR}/resource/*.rc)
FILE(GLOB_RECURSE QRC_FILES ${PROJECT_SOURCE_DIR}/resource/*.qrc)
FILE(GLOB_RECURSE UI_FILES ${PROJECT_SOURCE_DIR}/ui/*.ui)
FILE(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE_FILES)
include_directories(${PROJECT_SOURCE_DIR}/../LCore/include)
include_directories(${PROJECT_SOURCE_DIR}/../LWidget/include)



set(PROJECT_SOURCES
    main.cpp
    ${SOURCE_FILES}
    ${UI_FILES}
    ${RC_FILES}
    ${QRC_FILES}
    ${HEADER_FILES}
)

qt_add_executable(Demo ${PROJECT_SOURCES})

set_target_properties(Demo
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(Demo
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        LCore
        LWidget
)
