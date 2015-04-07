cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Sim_Gazebo )

#------------------------------------------------------
find_package( Pthread REQUIRED )

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO REQUIRED gazebo )
endif()
include_directories( ${GAZEBO_INCLUDE_DIRS} )
link_directories( ${GAZEBO_LIBRARY_DIRS} )

find_path( GZSERVER_BIN gzserver )
if(GZSERVER_BIN-NOTFOUND)
  message( WARNING "Could not find gzserver.  Make sure gazebo is fully installed before proceeding." )
endif()

set( GZSERVER_BIN "${GZSERVER_BIN}/gzserver" )
#message( WARNING "${GZSERVER_BIN}" )
add_definitions( -DGZSERVER_BIN="${GZSERVER_BIN}")

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
add_library( reveal_sim_gazebo SHARED ${HDRS} ${SRCS} )

add_dependencies( reveal_sim_gazebo 
  reveal_core
)

target_link_libraries( reveal_sim_gazebo 
  ${PTHREAD_LIBRARIES} 
  ${GAZEBO_LIBRARIES}
  reveal_core
)
#------------------------------------------------------
set_target_properties( reveal_sim_gazebo PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_sim_gazebo DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/sim/gazebo COMPONENT headers )

#TODO: automatically write a gazebo.h header in the sims directory that includes
#      all the relevant gazebo classes so that users can simply include 
#      Reveal/sim/gazebo.h to access all methods in gazebo sim rather than
#      having to drill down into Reveal/sim/gazebo/ to reference individual
#      files and classes
