cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Core )

#------------------------------------------------------

#------------------------------------------------------
find_package( Boost REQUIRED )
find_package( Protobuf REQUIRED )
find_package( ZeroMQ REQUIRED )

include_directories( 
  ${ZeroMQ_INCLUDE_DIRS}
)

link_directories(
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES}
)

#------------------------------------------------------
# header files
set( REVEAL_CORE_HEADERS 
  pointers.h
  user.h
  session.h
  authorization.h
  transport_exchange.h
  connection.h
  ipc.h
  state.h
  control.h
  link.h
  joint.h
  model.h
  digest.h
  scenario.h
  experiment.h
  trial.h
  solution.h
#  model_solution.h
  solution_set.h
  analyzer.h
  analysis.h
  log.h
)

# source files
set( REVEAL_CORE_SOURCES 
  transport_exchange.cpp
  connection.cpp
  ipc.cpp
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
install( FILES ${REVEAL_CORE_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/core COMPONENT headers )

