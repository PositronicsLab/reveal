cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Server )

#---------------Reveal Server---------------
find_package( Pthread REQUIRED )

# header files
set( REVEAL_SERVER_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/server.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/worker.h
)

# source files
set( REVEAL_SERVER_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp
)

# additional include paths
set( REVEAL_SERVER_INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# any libraries
set( REVEAL_SERVER_LIBRARIES
  ${PTHREAD_LIBRARIES}
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
  ${REVEAL_DB_LIBRARY}
  ${REVEAL_DB_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_SERVER_INCLUDE_DIRS} )

# build the executables

# build the tests
add_subdirectory( test )
