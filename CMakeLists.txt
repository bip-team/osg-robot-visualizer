cmake_minimum_required(VERSION 2.6)
project( osg-robot-visualizer )

set(PROJECT_NAME osg-robot-visualizer)

####################################
# Settings
####################################
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra")


####################################
# Dependencies
####################################

find_package(OpenSceneGraph REQUIRED osgViewer osgGA osgDB)

find_package(PkgConfig REQUIRED)
pkg_check_modules(YAMLCPP REQUIRED yaml-cpp)

include_directories(BEFORE SYSTEM "${OPENSCENEGRAPH_INCLUDE_DIRS}")
include_directories(BEFORE SYSTEM "${YAMLCPP_INCLUDE_DIRS}")
include_directories(BEFORE "${PROJECT_SOURCE_DIR}/src/")

link_directories("${YAMLCPP_LIBRARY_DIRS}")


####################################
# Compilation
####################################

add_executable(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/src/visualizer.cpp")
target_link_libraries(${PROJECT_NAME} ${YAMLCPP_LIBRARIES} ${OPENSCENEGRAPH_LIBRARIES})


####################################
# Installation
####################################

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
