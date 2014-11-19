cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Core )

#------------------------------------------------------
add_subdirectory( messages )

#------------------------------------------------------
find_package( Boost REQUIRED )
find_package( Protobuf REQUIRED )
find_package( ZeroMQ REQUIRED )

include_directories( 
  ${ZeroMQ_INCLUDE_DIRS}
#  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${REVEAL_MESSAGES_INCLUDE_DIR}
)

link_directories(
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES}
#  ${REVEAL_MESSAGES_LIBRARY}
)

#------------------------------------------------------
# header files
set( REVEAL_CORE_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/pointers.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/user.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/session.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/authorization.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/transport_exchange.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/connection.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/ipc.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/state.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/control.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/link.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/joint.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/model.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/digest.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/scenario.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/experiment.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/trial.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/solution.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/model_solution.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/solution_set.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analyzer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analysis.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/log.h
)

# source files
set( REVEAL_CORE_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/transport_exchange.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/connection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ipc.cpp
)

#------------------------------------------------------
# build the revealcore library
add_library( revealcore SHARED ${REVEAL_CORE_HEADERS} ${REVEAL_CORE_SOURCES} )
add_dependencies( revealcore revealmessages )

target_link_libraries( revealcore 
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES} 
)

target_link_libraries( revealcore revealmessages )

#------------------------------------------------------
# install
set_target_properties( revealcore PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealcore DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${REVEAL_CORE_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT headers )

#------------------------------------------------------
# create the reveal core include and library variables for future linking
set( REVEAL_CORE_INCLUDE_DIR
  ${REVEAL_MESSAGES_INCLUDE_DIR}   #for proto headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# set the variables in PARENT_SCOPE as well
set( REVEAL_CORE_INCLUDE_DIR ${REVEAL_CORE_INCLUDE_DIR} PARENT_SCOPE )

