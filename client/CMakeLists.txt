cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Client )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( Boost REQUIRED )

add_definitions( -DMAKE_BIN="${CMAKE_MAKE_PROGRAM}")
add_definitions( -DCMAKE_BIN="${CMAKE_COMMAND}")

find_path( GZSERVER_BIN gzserver )
#add_definitions( -DGZSERVER_BIN="${GZSERVER_PATH}/gzserver")
add_definitions( -DGZSERVER_BIN="/usr/local/bin/gzserver")

include_directories( 
  ${REVEAL_CORE_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

#------------------------------------------------------
# header files
set( REVEAL_CLIENT_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/client.h
)

# source files
set( REVEAL_CLIENT_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp
)

#------------------------------------------------------
add_executable( revealclient ${CMAKE_CURRENT_SOURCE_DIR}/reveal_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )

add_dependencies( revealclient 
  revealcore
)

target_link_libraries( revealclient 
  ${PTHREAD_LIBRARIES} 
  revealcore 
)

#------------------------------------------------------
set_target_properties( revealclient PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealclient DESTINATION ${BIN_INSTALL_DIR} )

# build the tests
#add_subdirectory( test )
#add_subdirectory( gazebo )

