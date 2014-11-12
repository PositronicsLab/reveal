project( Reveal )
cmake_minimum_required(VERSION 2.8.7)

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g ")

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO REQUIRED gazebo )
endif()
include_directories( ${GAZEBO_INCLUDE_DIRS} )
link_directories( ${GAZEBO_LIBRARY_DIRS} )

#if( PKG_CONFIG_FOUND )
#  pkg_check_modules( REVEAL_CORE REQUIRED reveal )
#endif()
#include_directories( ${REVEAL_CORE_INCLUDE_DIRS} )
#link_directories( ${REVEAL_CORE_LIBRARY_DIRS} )

set( LIBS 
  ${GAZEBO_LIBRARIES} 
  /usr/local/lib/librevealcore.so
) 

include_directories( /usr/local/include )

add_library( controller SHARED controller.cpp )
target_link_libraries( controller ${LIBS} )

add_library( world-plugin SHARED world_plugin.cpp )
target_link_libraries( world-plugin ${LIBS} )

# Copy data files
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/models DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/test.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
