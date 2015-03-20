cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Sim_Gazebo )


#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( Boost REQUIRED )

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO REQUIRED gazebo )
endif()
include_directories( ${GAZEBO_INCLUDE_DIRS} )
link_directories( ${GAZEBO_LIBRARY_DIRS} )

find_path( GZSERVER_BIN gzserver )
#add_definitions( -DGZSERVER_BIN="${GZSERVER_PATH}/gzserver")
add_definitions( -DGZSERVER_BIN="/usr/local/bin/gzserver")

#------------------------------------------------------
set( HDRS 
  gazebo.h
  helpers.h
)

set( SRCS 
  gazebo.cpp
  helpers.cpp
)

#------------------------------------------------------
add_library( revealsimgazebo SHARED ${HDRS} ${SRCS} )

add_dependencies( revealsimgazebo 
  revealcore
)

target_link_libraries( revealsimgazebo 
  ${PTHREAD_LIBRARIES} 
  ${GAZEBO_LIBRARIES}
  revealcore 
)
#------------------------------------------------------
set_target_properties( revealsimgazebo PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealsimgazebo DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/sim/gazebo COMPONENT headers )

#TODO: automatically write a gazebo.h header in the sims directory that includes
#      all the relevant gazebo classes so that users can simply include 
#      Reveal/sim/gazebo.h to access all methods in gazebo sim rather than
#      having to drill down into Reveal/sim/gazebo/ to reference individual
#      files and classes
