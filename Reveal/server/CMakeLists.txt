cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Server )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( uuid REQUIRED )

link_directories(
  ${PTHREAD_LIBRARIES}
  ${UUID_LIBRARIES}
)

#------------------------------------------------------
# header files
set( REVEAL_SERVER_HEADERS 
  identity.h
  server.h
  client_worker.h
)

# source files
set( REVEAL_SERVER_SOURCES 
  server.cpp
  client_worker.cpp
)

#------------------------------------------------------
add_executable( revealserver ${CMAKE_CURRENT_SOURCE_DIR}/reveal_server.cpp ${REVEAL_SERVER_HEADERS} ${REVEAL_SERVER_SOURCES} )

add_dependencies( revealserver 
  revealcore
  revealdb
)

target_link_libraries( revealserver
  ${PTHREAD_LIBRARIES}
  ${UUID_LIBRARIES}
  revealcore
  revealdb
  revealanalytics
)

#------------------------------------------------------
set_target_properties( revealserver PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealserver DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${REVEAL_CLIENT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/server COMPONENT headers )


# build the tests
#add_subdirectory( test )
