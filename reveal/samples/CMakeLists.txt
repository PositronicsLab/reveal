cmake_minimum_required(VERSION 2.8.7)

set( COMPONENT_NAME "samples" )
set( COMPONENT_DIR "${COMPONENT_NAME}")
project( "reveal_${COMPONENT_NAME}" )

#------------------------------------------------------
find_package( Boost REQUIRED COMPONENTS system filesystem )
find_package( ZeroMQ REQUIRED )
find_package( TinyXML REQUIRED )

include_directories( 
  ${ZeroMQ_INCLUDE_DIRS}
)

link_directories(
  ${ZeroMQ_LIBRARIES}
)

#------------------------------------------------------
# header files
set( HDRS 
  exchange.h
)

# source files
set( SRCS 
  exchange.cpp
)

#------------------------------------------------------
# build the revealcore library
add_library( ${PROJECT_NAME} SHARED ${HDRS} ${SRCS} )
add_dependencies( ${PROJECT_NAME}
  reveal_messages
)

#target_link_libraries( ${PROJECT_NAME}  
#  ${Boost_LIBRARIES}
#  ${ZeroMQ_LIBRARIES} 
#  ${TinyXML_LIBRARIES}
#)

target_link_libraries( ${PROJECT_NAME} 
  reveal_messages
)

#------------------------------------------------------
# install
set_target_properties( ${PROJECT_NAME} PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_DIR} COMPONENT headers )

#------------------------------------------------------
if( ${INSTALL_SAMPLES_SERVER} )
add_subdirectory( service )
endif()

if( ${INSTALL_SAMPLES_CLIENT} )
add_subdirectory( client )
endif()

