project( Reveal-Package-Weazelball )
cmake_minimum_required(VERSION 2.8.7)

#------------------------------------------------------------------------------
option( SERVER_SIDE "compile package as server side" OFF )
option( VISUALIZE_REAL_DATA "load real data file and visualize" OFF )

if( VISUALIZE_REAL_DATA )
  add_definitions( -DVISUALIZE_REAL_DATA )
endif()

#------------------------------------------------------------------------------
#server only
#add_definitions( -DANALYZER_PATH="${CMAKE_CURRENT_BUILD_DIR}" )
add_definitions( -DANALYZER_PATH="/home/james/osrf/Reveal/packages/weazelball/server/build/libweazelball-analyzer.so" )

#------------------------------------------------------------------------------

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

if( ${SERVER_SIDE} )
set( LIBS
  ${LIBS}
  /usr/local/lib/librevealdb.so     # include only for data generation
)
endif( ${SERVER_SIDE} )

include_directories( /usr/local/include )

set( HDRS 
  models.h
)

add_library( controller SHARED controller.cpp ${HDRS} )
target_link_libraries( controller ${LIBS} )

add_library( world-plugin SHARED world_plugin.cpp ${HDRS} )
target_link_libraries( world-plugin ${LIBS} )

# Copy data files
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/models DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/gazebo.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/reveal.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
