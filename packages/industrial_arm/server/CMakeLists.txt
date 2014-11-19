cmake_minimum_required( VERSION 2.8.7 )

project( Reveal_Pendulum_Scenario )

find_package( Boost REQUIRED )

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO gazebo )
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
  /usr/local/lib/librevealdb.so
  /usr/local/lib/librevealanalytics.so
) 

include_directories( /usr/local/include )



#set( HDRS
#  ${CMAKE_CURRENT_SOURCE_DIR}/pendulum.h
#)

#set( SRCS
#  ${CMAKE_CURRENT_SOURCE_DIR}/pendulum.cpp
#  ${REVEAL_CLIENT_SOURCES}
#)

#set( INCLUDE_DIRS
#  ${REVEAL_CORE_INCLUDE_DIR}
#  ${REVEAL_DB_INCLUDE_DIR}
#  ${REVEAL_ANALYTICS_INCLUDE_DIR}
#  ${REVEAL_CLIENT_INCLUDE_DIR}
#)

#set( LIBS 
#  ${PTHREAD_LIBRARIES}
#  ${REVEAL_CORE_LIBRARY}
#  ${REVEAL_CORE_LIBRARIES}
#  ${REVEAL_DB_LIBRARY}
#  ${REVEAL_DB_LIBRARIES}
#  ${REVEAL_ANALYTICS_LIBRARY}
#  ${REVEAL_ANALYTICS_LIBRARIES}
#  ${REVEAL_CLIENT_LIBRARY}
#  ${REVEAL_CLIENT_LIBRARIES}
#)

#include_directories( ${INCLUDE_DIRS} )

add_executable( industrial-arm-data-gen data_generator.cpp )
target_link_libraries( industrial-arm-data-gen ${LIBS} )

add_library( industrial-arm-analyzer "MODULE" plugin.cpp )
target_link_libraries( industrial-arm-analyzer ${LIBS} )

