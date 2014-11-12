cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Server )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( uuid REQUIRED )

include_directories( 
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
  ${REVEAL_ANALYTICS_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

link_directories(
  ${PTHREAD_LIBRARIES}
  ${UUID_LIBRARIES}
)

#------------------------------------------------------
# header files
set( REVEAL_SERVER_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/identity.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/server.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/client_worker.h
)

# source files
set( REVEAL_SERVER_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client_worker.cpp
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


# build the tests
#add_subdirectory( test )
