cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Client )

#------------------------------------------------------
find_package( Pthread REQUIRED )

#------------------------------------------------------
# common header files
set( REVEAL_CLIENT_HEADERS 
  client.h
)

# common source files
set( REVEAL_CLIENT_SOURCES 
  client.cpp
)

#------------------------------------------------------
add_executable( reveal_client ${CMAKE_CURRENT_SOURCE_DIR}/reveal_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )

add_dependencies( reveal_client 
  reveal_core
  reveal_sim_gazebo
)
target_link_libraries( reveal_client 
  ${PTHREAD_LIBRARIES} 
  reveal_core
  reveal_sim_gazebo 
)

#------------------------------------------------------
set_target_properties( reveal_client PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_client DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${REVEAL_CLIENT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/client COMPONENT headers )

