cmake_minimum_required(VERSION 2.8.7)

set( COMPONENT_NAME "client" )
set( COMPONENT_DIR "samples/${COMPONENT_NAME}")
project( "reveal_samples_${COMPONENT_NAME}" )

#------------------------------------------------------
find_package( Pthread REQUIRED )

#------------------------------------------------------
# common header files
set( HDRS 
  client.h
)

# common source files
set( SRCS 
  client.cpp
)

set( LIBS 
  ${PTHREAD_LIBRARIES} 
  reveal_core
  reveal_sim
  reveal_samples 
)

#------------------------------------------------------
add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/samples_client.cpp ${HDRS} ${SRCS} )
add_dependencies( ${PROJECT_NAME} ${LIBS} )
target_link_libraries( ${PROJECT_NAME} ${LIBS} )

#------------------------------------------------------
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_DIR} COMPONENT headers )

