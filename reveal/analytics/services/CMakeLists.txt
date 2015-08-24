cmake_minimum_required(VERSION 2.8.7)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg ")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

set( COMPONENT_NAME "server" )
set( COMPONENT_DIR "analytics/${COMPONENT_NAME}" )
project( "reveal_analytics_${COMPONENT_NAME}" )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( Boost REQUIRED COMPONENTS system thread )

link_directories(
  ${PTHREAD_LIBRARIES}
)

#------------------------------------------------------
# header files
set( HDRS 
  service.h
  worker.h
)

# source files
set( SRCS 
  service.cpp
  worker.cpp
)

#------------------------------------------------------
add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp ${HDRS} ${SRCS} )

add_dependencies( ${PROJECT_NAME} 
  reveal_core
  reveal_db
  reveal_analytics
)

target_link_libraries( ${PROJECT_NAME}
  ${PTHREAD_LIBRARIES}
  ${Boost_LIBRARIES}
  reveal_core
  reveal_db
  reveal_analytics
)

#------------------------------------------------------
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_DIR} COMPONENT headers )

