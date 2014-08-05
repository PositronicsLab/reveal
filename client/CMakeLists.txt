cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Client )

#---------------Reveal Client---------------
# header files
set( REVEAL_CLIENT_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/client.h
)

# source files
set( REVEAL_CLIENT_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp
)

# additional include paths
set( REVEAL_CLIENT_INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# any libraries
set( REVEAL_CLIENT_LIBRARIES
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_CLIENT_INCLUDE_DIRS} )

set( REVEAL_CLIENT_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# build the executables

# build the tests
add_subdirectory( test )

set( REVEAL_CLIENT_INCLUDE_DIR ${REVEAL_CLIENT_INCLUDE_DIR} PARENT_SCOPE )
set( REVEAL_CLIENT_SOURCES ${REVEAL_CLIENT_SOURCES} PARENT_SCOPE )
