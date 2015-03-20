cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Core_Messages )

find_package( Protobuf REQUIRED )

#add_subdirectory( Reveal/messages )
# add any messages to be compiled here
set( REVEAL_MESSAGES_PROTOS
  data.proto
  auth.proto
  net.proto
)

#------------------------------------------------------
# add the protobuf include directories
include_directories( ${PROTOBUF_INCLUDE_DIRS} )
# generate the message classes
PROTOBUF_GENERATE_CPP( PROTO_SRCS PROTO_HDRS ${REVEAL_MESSAGES_PROTOS} )
# Note: above generates to build folder.  protoc has --cpp_out parameter to 
# direct this location, but macro assumes build folder.  To put into a more
# controlled location, write a macro that exposes --cpp_out and set
# REVEAL_MESSAGES_INCLUDE_DIRS

#------------------------------------------------------
set( REVEAL_MESSAGES_LIBRARIES
  ${PROTOBUF_LIBRARY}
)

#------------------------------------------------------
# build the message library
add_library(reveal_messages SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(reveal_messages ${REVEAL_MESSAGES_LIBRARIES})

#------------------------------------------------------
set_target_properties( reveal_messages PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_messages DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${PROTO_HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/messages COMPONENT headers )

