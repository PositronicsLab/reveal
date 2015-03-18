cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Client )

option( GAZEBO "compile package with Gazebo support" OFF )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( Boost REQUIRED )

if( GAZEBO )
  include( FindPkgConfig )
  if( PKG_CONFIG_FOUND )
    pkg_check_modules( GAZEBO REQUIRED gazebo )
  endif()
  include_directories( ${GAZEBO_INCLUDE_DIRS} )
  link_directories( ${GAZEBO_LIBRARY_DIRS} )
endif()


add_definitions( -DMAKE_BIN="${CMAKE_MAKE_PROGRAM}")
add_definitions( -DCMAKE_BIN="${CMAKE_COMMAND}")

if( GAZEBO )
  find_path( GZSERVER_BIN gzserver )
  #add_definitions( -DGZSERVER_BIN="${GZSERVER_PATH}/gzserver")
  add_definitions( -DGZSERVER_BIN="/usr/local/bin/gzserver")
endif()

#------------------------------------------------------
# common header files
set( REVEAL_CLIENT_HEADERS 
  simulator.h
  client.h
  package.h
)

# common source files
set( REVEAL_CLIENT_SOURCES 
  client.cpp
  package.cpp
)

if( GAZEBO )
  # append gazebo specific header files
  set( REVEAL_CLIENT_HEADERS
    ${REVEAL_CLIENT_HEADERS}
    gazebo.h
  )

  # append gazebo specific source files
  set( REVEAL_CLIENT_SOURCES 
    ${REVEAL_CLIENT_SOURCES}
    gazebo.cpp
  )
endif()

#------------------------------------------------------
add_executable( revealclient ${CMAKE_CURRENT_SOURCE_DIR}/reveal_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )

add_dependencies( revealclient 
  revealcore
)
target_link_libraries( revealclient 
  ${PTHREAD_LIBRARIES} 
  revealcore 
)
if( GAZEBO )
  target_link_libraries( revealclient ${GAZEBO_LIBRARIES} )
endif()
#------------------------------------------------------
set_target_properties( revealclient PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealclient DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${REVEAL_CLIENT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/client COMPONENT headers )

#------------------------------------------------------
#------------------------------------------------------
# Note: this is only valid if the db is local.  May belong in a different 
# package as we don't want to have any dependency to database in general client
# build tree
#------------------------------------------------------
# header files
set( REVEAL_IMPORT_HEADERS 
  importer.h
)

# source files
set( REVEAL_IMPORT_SOURCES 
  importer.cpp
)

add_executable( revealimport ${CMAKE_CURRENT_SOURCE_DIR}/reveal_import.cpp ${REVEAL_IMPORT_HEADERS} ${REVEAL_IMPORT_SOURCES} )

add_dependencies( revealimport 
  revealcore
  revealdb
)
target_link_libraries( revealimport
#  ${TinyXML_LIBRARIES}
#  ${PTHREAD_LIBRARIES} 
  revealcore 
  revealdb
)

#------------------------------------------------------
set_target_properties( revealimport PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealimport DESTINATION ${BIN_INSTALL_DIR} )
#install( FILES ${REVEAL_CLIENT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/client COMPONENT headers )

# build the tests
#add_subdirectory( test )

