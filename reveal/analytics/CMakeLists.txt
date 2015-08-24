cmake_minimum_required(VERSION 2.8.7)

set( COMPONENT_NAME "analytics" )
set( COMPONENT_DIR "${COMPONENT_NAME}")
project( "reveal_${COMPONENT_NAME}" )

#------------------------------------------------------
#find_package( Boost REQUIRED COMPONENTS system filesystem )
#find_package( ZeroMQ REQUIRED )
#find_package( TinyXML REQUIRED )
find_package( Pthread REQUIRED )
find_package( DL REQUIRED )

include_directories( 
#  ${ZeroMQ_INCLUDE_DIRS}
)

link_directories(
#  ${ZeroMQ_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
)

#------------------------------------------------------
# header files
set( HDRS 
  exchange.h
  types.h
  module.h
  plugin.h
  script.h
  worker.h
)

# source files
set( SRCS 
  exchange.cpp
  plugin.cpp
  script.cpp
  worker.cpp
)

#------------------------------------------------------
# build the revealcore library
add_library( ${PROJECT_NAME} SHARED ${HDRS} ${SRCS} )
add_dependencies( ${PROJECT_NAME}
  reveal_messages
  reveal_core
  reveal_db
)

target_link_libraries( ${PROJECT_NAME}  
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
  reveal_messages
  reveal_core
  reveal_db
#  ${Boost_LIBRARIES}
#  ${ZeroMQ_LIBRARIES} 
#  ${TinyXML_LIBRARIES}
)

#------------------------------------------------------
# install
set_target_properties( ${PROJECT_NAME} PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_DIR} COMPONENT headers )

#------------------------------------------------------

#if( ${INSTALL_ANALYTICS_SERVER} )
#  add_subdirectory( services )
#endif( ${INSTALL_ANALYTICS_SERVER} )

#if( ${INSTALL_ANALYTICS_CLIENT} )
#add_subdirectory( client )
#endif( ${INSTALL_ANALYTICS_CLIENT} )
