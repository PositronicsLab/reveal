cmake_minimum_required( VERSION 2.8.7 )
project( Reveal-Analyzer-Industrial_Arm )

find_package( Boost REQUIRED )

#include( FindPkgConfig )
#if( PKG_CONFIG_FOUND )
#  pkg_check_modules( GAZEBO gazebo )
#endif()
#include_directories( ${GAZEBO_INCLUDE_DIRS} )
#link_directories( ${GAZEBO_LIBRARY_DIRS} )

set( CMAKE_MODULES_DIR ${PROJECT_SOURCE_DIR}/../../../cmake_modules CACHE PATH "CMake scripts" )
set( CMAKE_MODULE_PATH ${CMAKE_MODULES_DIR} ${CMAKE_MODULE_PATH} )

#message( WARNING "CMAKE_MODULES_DIR: ${CMAKE_MODULES_DIR}" )

find_package( Ravelin REQUIRED )
#include( FindPkgConfig )
#if( PKG_CONFIG_FOUND )
#  pkg_check_modules( RAVELIN ravelin )
#endif()
include_directories( ${RAVELIN_INCLUDE_DIRS} )
link_directories( ${RAVELIN_LIBRARY_DIRS} )

find_package( Reveal REQUIRED )
include_directories( ${REVEAL_INCLUDE_DIRS} )
link_directories( ${REVEAL_LIBRARY_DIRS} )

set( LIBS 
  ${REVEAL_LIBRARIES}
  ${RAVELIN_LIBRARIES} 
) 

#message( WARNING "RAVELIN_LIBRARIES: ${RAVELIN_LIBRARIES}" )
#message( WARNING "RAVELIN_INCLUDE_DIRS: ${RAVELIN_INCLUDE_DIRS}" )
#message( WARNING "RAVELIN_LIBRARY_DIRS: ${RAVELIN_LIBRARY_DIRS}" )

add_library( industrial-arm-analyzer "MODULE" analyzer.cpp )
target_link_libraries( industrial-arm-analyzer ${LIBS} )

