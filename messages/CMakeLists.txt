# set the messages include path
set( REVEAL_MESSAGES_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "Reveal protobuf files." )

# set the include path for any intermessage dependencies
include_directories( ${REVEAL_MESSAGES_INCLUDE_DIRS} )

# add any messages to be compiled here
set( REVEAL_MESSAGES
  net.proto
)

# add the protobuf include directories
include_directories( ${PROTOBUF_INCLUDE_DIRS} )
# generate the message classes
PROTOBUF_GENERATE_CPP( PROTO_SRCS PROTO_HDRS ${REVEAL_MESSAGES} )

# build the message library
add_library(messages STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(messages ${PROTOBUF_LIBRARY})

# export the library path
set( REVEAL_MESSAGE_LIBRARY 
  ${CMAKE_CURRENT_BINARY_DIR}/libmessages.a
  PARENT_SCOPE 
)
