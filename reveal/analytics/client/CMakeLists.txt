cmake_minimum_required(VERSION 2.8.7)

set( COMPONENT_NAME "client" )
set( COMPONENT_DIR "analytics/${COMPONENT_NAME}")
project( "reveal_analytics_${COMPONENT_NAME}" )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( DL REQUIRED )

link_directories(
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
)

#------------------------------------------------------
# header files
set( HDRS 
client.h
#  service.h
#  worker.h
)

# source files
set( SRCS 
  client.cpp
#  service.cpp
#  worker.cpp
)

#------------------------------------------------------
add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/analyzer_client.cpp ${HDRS} ${SRCS} )

add_dependencies( ${PROJECT_NAME} 
  reveal_messages 
  reveal_core
  reveal_db 
  reveal_analytics
)

target_link_libraries( ${PROJECT_NAME}
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
  reveal_messages
  reveal_core
  reveal_db
  reveal_analytics
)

#------------------------------------------------------
# install
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_NAME} COMPONENT headers )

