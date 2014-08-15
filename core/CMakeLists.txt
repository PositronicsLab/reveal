cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Core )

#---------------Reveal Messages-------------
find_package( Protobuf REQUIRED )

# add any messages to be compiled here
set( REVEAL_MESSAGES_PROTOS
  messages/data.proto
  messages/net.proto
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
  ${LIBRARY_OUTPUT_PATH}/librevealmessages.so
)

# have to include build directory due to Note on PROTOBUF_GENERATE_CPP above
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

#---------------Reveal Core-----------------
# find all packages
find_package( Protobuf REQUIRED )
find_package( ZeroMQ REQUIRED )
#find_package( Boost REQUIRED )

# header files
set( REVEAL_CORE_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/pointers.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/transport_exchange.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/connection.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/state.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/control.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/digest.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/scenario.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/trial.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/solution.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/model_solution.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/solution_set.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analyzer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analysis.h
)

# source files
set( REVEAL_CORE_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/transport_exchange.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/connection.cpp
)

# additional include paths
set( REVEAL_CORE_INCLUDE_DIRS
  ${ZeroMQ_INCLUDE_DIRS}
#  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# any libraries
set( REVEAL_CORE_LIBRARIES
#  ${Boost_LIBRARIES}
  ${REVEAL_MESSAGES_LIBRARY}
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_CORE_INCLUDE_DIRS} )

# build the revealcore library
add_library( revealcore SHARED ${REVEAL_CORE_HEADERS} ${REVEAL_CORE_SOURCES} )
add_dependencies( revealcore revealmessages )
#add_dependencies( revealcore ${REVEAL_MESSAGES_LIBRARY} )
#target_link_libraries( revealcore LINK_PRIVATE ${REVEAL_CORE_LIBARIES})
target_link_libraries(revealcore LINK_PRIVATE ${ZeroMQ_LIBRARIES})
target_link_libraries(revealcore LINK_PRIVATE ${PROTOBUF_LIBRARIES})
target_link_libraries(revealcore LINK_PRIVATE ${REVEAL_MESSAGES_LIBRARY})

# create the reveal core include and library variables for future linking
set( REVEAL_CORE_INCLUDE_DIR
  ${CMAKE_CURRENT_BINARY_DIR}   #for proto headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set( REVEAL_CORE_LIBRARY
  ${LIBRARY_OUTPUT_PATH}/librevealcore.so
)

# set the variables in PARENT_SCOPE as well
set( REVEAL_CORE_LIBRARIES ${REVEAL_CORE_LIBRARIES} PARENT_SCOPE )
set( REVEAL_CORE_INCLUDE_DIR ${REVEAL_CORE_INCLUDE_DIR} PARENT_SCOPE )
set( REVEAL_CORE_LIBRARY ${REVEAL_CORE_LIBRARY} PARENT_SCOPE )

