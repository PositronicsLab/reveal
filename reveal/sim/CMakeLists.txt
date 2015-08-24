cmake_minimum_required(VERSION 2.8.7)

set( COMPONENT_NAME "sim" )
set( COMPONENT_DIR "${COMPONENT_NAME}")
project( "reveal_${COMPONENT_NAME}" )

#------------------------------------------------------
#add_definitions( -DMAKE_BIN="${CMAKE_MAKE_PROGRAM}")
#add_definitions( -DCMAKE_BIN="${CMAKE_COMMAND}")

#------------------------------------------------------
#find_package( Boost REQUIRED COMPONENTS system filesystem )
#find_package( ZeroMQ REQUIRED )
#find_package( TinyXML REQUIRED )

#include_directories( 
#  ${ZeroMQ_INCLUDE_DIRS}
#)

#link_directories(
#  ${ZeroMQ_LIBRARIES}
#)

#------------------------------------------------------
# header files
set( HDRS 
  adapter.h
)

# source files
set( SRCS 
  adapter.cpp
)

set( LIBS 
  reveal_core
)

#------------------------------------------------------
# build the revealcore library
add_library( ${PROJECT_NAME} SHARED ${HDRS} ${SRCS} )
add_dependencies( ${PROJECT_NAME} ${LIBS} )

#target_link_libraries( ${PROJECT_NAME}  
#  ${Boost_LIBRARIES}
#  ${ZeroMQ_LIBRARIES} 
#  ${TinyXML_LIBRARIES}
#)

target_link_libraries( ${PROJECT_NAME} ${LIBS} )

#------------------------------------------------------
# install
set_target_properties( ${PROJECT_NAME} PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_DIR} COMPONENT headers )

#------------------------------------------------------
#unit testing
add_executable( adapter_TEST adapter_TEST.cpp ${HDRS} ${SRCS} )
target_link_libraries( adapter_TEST ${LIBS} )
