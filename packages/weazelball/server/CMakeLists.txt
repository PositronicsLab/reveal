cmake_minimum_required( VERSION 2.8.7 )
project( Reveal-Package-Weazelball )

find_package( Boost REQUIRED )

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO gazebo )
endif()
include_directories( ${GAZEBO_INCLUDE_DIRS} )
link_directories( ${GAZEBO_LIBRARY_DIRS} )

find_package( Reveal REQUIRED )
include_directories( ${REVEAL_INCLUDE_DIRS} )
link_directories( ${REVEAL_LIBRARY_DIRS} )

set( LIBS 
  ${GAZEBO_LIBRARIES} 
  ${REVEAL_LIBRARIES}
  #/usr/local/lib/librevealcore.so
  #/usr/local/lib/librevealdb.so
  #/usr/local/lib/librevealanalytics.so
) 

add_library( weazelball-analyzer "MODULE" analyzer.cpp )
target_link_libraries( weazelball-analyzer ${LIBS} )

