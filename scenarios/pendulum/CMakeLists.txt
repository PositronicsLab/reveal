cmake_minimum_required( VERSION 2.8.7 )

project( Reveal_Pendulum_Scenario )

find_package( Boost REQUIRED )

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO gazebo )
endif()
include_directories( ${GAZEBO_INCLUDE_DIRS} )
link_directories( ${GAZEBO_LIBRARY_DIRS} )

set( HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/pendulum.h
)

set( SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/pendulum.cpp
  ${REVEAL_CLIENT_SOURCES}
)

set( INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
  ${REVEAL_ANALYTICS_INCLUDE_DIR}
  ${REVEAL_CLIENT_INCLUDE_DIR}
)

set( LIBS 
#  ${PTHREAD_LIBRARIES}
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
  ${REVEAL_DB_LIBRARY}
  ${REVEAL_DB_LIBRARIES}
  ${REVEAL_ANALYTICS_LIBRARY}
  ${REVEAL_ANALYTICS_LIBRARIES}
#  ${REVEAL_CLIENT_LIBRARY}
#  ${REVEAL_CLIENT_LIBRARIES}
)

include_directories( ${INCLUDE_DIRS} )

#add_library( pendulum-controller SHARED pendulum.cpp)
#target_link_libraries( pendulum-controller ${GAZEBO_LIBRARIES} )

add_executable( test-pendulum test.cpp ${HDRS} ${SRCS} )
target_link_libraries( test-pendulum ${LIBS} )

#add_executable( test-pendulum-client client.cpp ${HDRS} ${SRCS} )
#target_link_libraries( test-pendulum-client ${LIBS} )

add_executable( test-pendulum-data-gen data_generator.cpp ${HDRS} ${SRCS} )
target_link_libraries( test-pendulum-data-gen ${LIBS} )

add_library( test-pendulum-analyzer-plugin "MODULE" plugin.cpp ${HDRS} ${SRCS} )
target_link_libraries( test-pendulum-analyzer-plugin ${LIBS} )

