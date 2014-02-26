# For Debug Build :: -DCMAKE_BUILD_TYPE=Debug

project( reveal )
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# make available any custom .cmakes
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

# find all packages
find_package( ZeroMQ REQUIRED )
find_package( Protobuf REQUIRED )

# compile the message library here so files exist for include and linking
add_subdirectory( messages )

# header files
set( REVEAL_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/reveal/server.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/reveal/client.h
)

# source files
set( REVEAL_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp
)

# additional include paths
set( REVEAL_INCLUDE_DIRS
  ${PROTOBUF_INCLUDE_DIR}
  ${ZeroMQ_INCLUDE_DIRS}
  ${REVEAL_MESSAGES_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# any libraries
set( REVEAL_LIBRARIES
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES}
  ${REVEAL_MESSAGE_LIBRARY}
)

# add the include paths
include_directories( ${REVEAL_INCLUDE_DIRS} )

# build the executables
# build the executables in test
add_subdirectory( test )

