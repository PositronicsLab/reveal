cmake_minimum_required(VERSION 2.8.7)
project( Reveal-Package-Weazelball )

#------------------------------------------------------------------------------
option( SERVER_SIDE "compile package as server side" OFF )

#------------------------------------------------------------------------------
#server only
## Note: hackish.  We can't necessarily guarantee where this will be based on
#  relative path, but good enough for now.
add_definitions( -DANALYZER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../server/build/libweazelball-analyzer.so" )

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
#)
#endif( ${SERVER_SIDE} )

#include_directories( /usr/local/include )

set( HDRS 
  models.h
  weazelball.h
)

add_library( controller SHARED controller.cpp ${HDRS} )
target_link_libraries( controller ${LIBS} )

add_library( vicon-visualization-controller SHARED visualization_controller.cpp ${HDRS} )
target_link_libraries( vicon-visualization-controller ${LIBS} )

add_library( postprocess-controller SHARED postprocess_controller.cpp ${HDRS} )
target_link_libraries( postprocess-controller ${LIBS} )

add_library( world-plugin SHARED world_plugin.cpp ${HDRS} )
target_link_libraries( world-plugin ${LIBS} )

# Copy data files
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/models DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/gazebo.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/reveal.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

# designed to work in conjunction with the vicon-visualization-controller
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/visualize.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

# designed to work in conjunction with the postprocess-controller
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/postprocess.world DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
