cmake_minimum_required(VERSION 2.8.7)
project( Reveal-Package-Industrial_Arm )

#------------------------------------------------------------------------------
option( SERVER_SIDE "compile package as server side" OFF )

#------------------------------------------------------------------------------
option( DATA_GENERATION "compile package for data generation" OFF )
if( DATA_GENERATION ) 
  add_definitions( -DDATA_GENERATION)
endif()

#------------------------------------------------------------------------------
#server only
## Note: hackish.  We can't necessarily guarantee where this will be based on
#  relative path, but good enough for now.
add_definitions( -DANALYZER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../server/build/libindustrial-arm-analyzer.so" )

#------------------------------------------------------------------------------

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g ")

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO REQUIRED gazebo )
endif()
include_directories( ${GAZEBO_INCLUDE_DIRS} )
link_directories( ${GAZEBO_LIBRARY_DIRS} )

find_package( Reveal REQUIRED )
include_directories( ${REVEAL_INCLUDE_DIRS} )
link_directories( ${REVEAL_LIBRARY_DIRS} )

set( LIBS 
  ${GAZEBO_LIBRARIES}
  ${REVEAL_LIBRARIES} 
) 

#if( ${SERVER_SIDE} )
#set( LIBS
#  ${LIBS}
#  #/usr/local/lib/librevealdb.so     # include only for data generation
#)
#endif( ${SERVER_SIDE} )

#include_directories( /usr/local/include )

set( HDRS 
  arm_controller.h
  models.h
)

add_library( gz-arm-plugin SHARED gazebo_arm_plugin.cpp ${HDRS} )
target_link_libraries( gz-arm-plugin ${LIBS} )

add_library( gz-world-plugin SHARED gazebo_world_plugin.cpp ${HDRS} )
target_link_libraries( gz-world-plugin ${LIBS} )

# Copy data files
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/models DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/gazebo.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/reveal.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
