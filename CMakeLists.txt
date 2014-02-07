# For Debug Build :: -DCMAKE_BUILD_TYPE=Debug

project( reveal )
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

find_package( ZeroMQ REQUIRED )
find_package( Protobuf REQUIRED )

set( REVEAL_MESSAGES
  protocols.proto
)
include_directories( ${PROTOBUF_INCLUDE_DIRS} )
PROTOBUF_GENERATE_CPP( PROTO_SRCS PROTO_HDRS ${REVEAL_MESSAGES} )

set( REVEAL_HEADERS 
  ${PROTO_HDRS}
)

set( REVEAL_SOURCES 
  ${PROTO_SRCS}
)

set( REVEAL_INCLUDE_DIRS
  ${PROTOBUF_INCLUDE_DIR}
  ${ZeroMQ_INCLUDE_DIRS}
)

set( REVEAL_LIBRARIES
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES}
)

include_directories( ${REVEAL_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( protobuf_test ${CMAKE_CURRENT_SOURCE_DIR}/protobuf_test.cpp ${REVEAL_HEADERS} ${REVEAL_SOURCES} )
target_link_libraries( protobuf_test ${REVEAL_LIBRARIES} )

add_executable( zeromq_test ${CMAKE_CURRENT_SOURCE_DIR}/zeromq_test.cpp ${REVEAL_HEADERS} ${REVEAL_SOURCES} )
target_link_libraries( zeromq_test ${REVEAL_LIBRARIES} )


