cmake_minimum_required( VERSION 2.8 )

project( Reveal_Block-Grasp_Scenario )

include( FindPkgConfig )
if( PKG_CONFIG_FOUND )
  pkg_check_modules( GAZEBO gazebo )
endif()
include_directories( ${GAZEBO_INCLUDE_DIRS} )
link_directories( ${GAZEBO_LIBRARY_DIRS} )

add_library( block-grasp-controller SHARED block_grasp_controller.cpp)
target_link_libraries( block-grasp-controller ${GAZEBO_LIBRARIES} )

#TODO
# add analyzer
# add data generator
# add test client
