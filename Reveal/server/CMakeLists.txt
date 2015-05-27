cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Server )

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg ")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( Boost REQUIRED COMPONENTS system thread )

link_directories(
  ${PTHREAD_LIBRARIES}
)

#------------------------------------------------------
# header files
set( REVEAL_SERVER_HEADERS 
  identity.h
  server.h
  worker.h
)

# source files
set( REVEAL_SERVER_SOURCES 
  server.cpp
  worker.cpp
)

#------------------------------------------------------
add_executable( reveal_server ${CMAKE_CURRENT_SOURCE_DIR}/reveal_server.cpp ${REVEAL_SERVER_HEADERS} ${REVEAL_SERVER_SOURCES} )

add_dependencies( reveal_server 
  reveal_core
  reveal_db
)

target_link_libraries( reveal_server
  ${PTHREAD_LIBRARIES}
  ${Boost_LIBRARIES}
  reveal_core
  reveal_db
  reveal_analytics
)

#------------------------------------------------------
set_target_properties( reveal_server PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_server DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${REVEAL_SERVER_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/server COMPONENT headers )


# build the tests
#add_subdirectory( test )
