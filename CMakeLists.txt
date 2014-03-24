# For Debug Build :: -DCMAKE_BUILD_TYPE=Debug

project( Reveal )
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# make available any custom .cmakes
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

#---------------Reveal Messages-------------
find_package( Protobuf REQUIRED )

# add any messages to be compiled here
set( REVEAL_MESSAGES_PROTOS
  core/messages/net.proto
)

# add the protobuf include directories
include_directories( ${PROTOBUF_INCLUDE_DIRS} )
# generate the message classes
PROTOBUF_GENERATE_CPP( PROTO_SRCS PROTO_HDRS ${REVEAL_MESSAGES_PROTOS} )
# Note: above generates to build folder.  protoc has --cpp_out parameter to 
# direct this location, but macro assumes build folder.  To put into a more
# controlled location, write a macro that exposes --cpp_out and set
# REVEAL_MESSAGES_INCLUDE_DIRS

set( REVEAL_MESSAGES_LIBRARIES
  ${PROTOBUF_LIBRARY}
)

# build the message library
add_library(revealmessages SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(revealmessages ${REVEAL_MESSAGES_LIBRARIES})

# export the library path
set( REVEAL_MESSAGES_LIBRARY 
  ${CMAKE_CURRENT_BINARY_DIR}/librevealmessages.so
)

# have to include build directory due to Note on PROTOBUF_GENERATE_CPP above
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

#---------------Reveal Core-----------------
# find all packages
find_package( Protobuf REQUIRED )
find_package( ZeroMQ REQUIRED )
find_package( Boost REQUIRED )

# header files
set( REVEAL_CORE_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/protocol_manager.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/connection.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/state.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/control.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/scenario.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/trial.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/solution.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/server_message.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/client_message.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Reveal/pendulum.h
)

# source files
set( REVEAL_CORE_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/core/src/connection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core/src/server_message.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core/src/client_message.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core/src/pendulum.cpp
)

# additional include paths
set( REVEAL_CORE_INCLUDE_DIRS
  ${ZeroMQ_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
#  ${REVEAL_MESSAGES_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/
)

# any libraries
set( REVEAL_CORE_LIBRARIES
  ${Boost_LIBRARIES}
  ${REVEAL_MESSAGES_LIBRARY}
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_CORE_INCLUDE_DIRS} )

# build the revealcore library
add_library( revealcore SHARED ${REVEAL_CORE_HEADERS} ${REVEAL_CORE_SOURCES} )
add_dependencies( revealcore revealmessages )
target_link_libraries( revealcore ${REVEAL_CORE_LIBARIES})

# create the reveal core library variable for future dependency linking
set( REVEAL_CORE_LIBRARY
  ${CMAKE_CURRENT_BINARY_DIR}/librevealcore.so
)

#---------------Reveal DB-------------------
# find all packages
#find_package( MongoDB REQUIRED )

#---------------Reveal Server---------------

# header files
set( REVEAL_SERVER_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/server/include/Reveal/server.h
)

# source files
set( REVEAL_SERVER_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/server/src/server.cpp
)

# additional include paths
set( REVEAL_SERVER_INCLUDE_DIRS
#  ${REVEAL_CORE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/server/include/
)

# any libraries
set( REVEAL_SERVER_LIBRARIES
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_SERVER_INCLUDE_DIRS} )

# build the executables in test
add_executable( test_reveal_server ${CMAKE_CURRENT_SOURCE_DIR}/server/test/test_reveal_server.cpp ${REVEAL_SERVER_HEADERS} ${REVEAL_SERVER_SOURCES} )
add_dependencies( test_reveal_server revealcore )
target_link_libraries( test_reveal_server ${REVEAL_SERVER_LIBRARIES} )

#---------------Reveal Client---------------

# header files
set( REVEAL_CLIENT_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/client/include/Reveal/client.h
)

# source files
set( REVEAL_CLIENT_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/client/src/client.cpp
)

# additional include paths
set( REVEAL_CLIENT_INCLUDE_DIRS
#  ${REVEAL_CORE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/client/include/
)

# any libraries
set( REVEAL_CLIENT_LIBRARIES
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_CLIENT_INCLUDE_DIRS} )

# build the executables

# build the executables in test
add_executable( test_reveal_client ${CMAKE_CURRENT_SOURCE_DIR}/client/test/test_reveal_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )
add_dependencies( test_reveal_client revealcore )
target_link_libraries( test_reveal_client ${REVEAL_CLIENT_LIBRARIES} )

