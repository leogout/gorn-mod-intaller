cmake_minimum_required(VERSION 3.10)
project(gorn_mod_intaller)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(RESOURCE_FOLDER res)
set(RESOURCE_FILES ${RESOURCE_FOLDER}/resources.qrc)

set(SOURCE_FILES
        src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${RESOURCE_FILES})

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets)

