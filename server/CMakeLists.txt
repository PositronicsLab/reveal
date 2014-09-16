cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Server )

#---------------Reveal Server---------------
find_package( Pthread REQUIRED )
find_package( uuid REQUIRED )

# header files
set( REVEAL_SERVER_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/server.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/client_worker.h
)

# source files
set( REVEAL_SERVER_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client_worker.cpp
)

# additional include paths
set( REVEAL_SERVER_INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
  ${REVEAL_ANALYTICS_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# any libraries
set( REVEAL_SERVER_LIBRARIES
  ${PTHREAD_LIBRARIES}
  ${UUID_LIBRARIES}
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
  ${REVEAL_DB_LIBRARY}
  ${REVEAL_DB_LIBRARIES}
  ${REVEAL_ANALYTICS_LIBRARY}
  ${REVEAL_ANALYTICS_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_SERVER_INCLUDE_DIRS} )

# build the executables

# build the tests
add_subdirectory( test )
