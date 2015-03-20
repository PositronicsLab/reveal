cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Core )

#------------------------------------------------------
add_definitions( -DMAKE_BIN="${CMAKE_MAKE_PROGRAM}")
add_definitions( -DCMAKE_BIN="${CMAKE_COMMAND}")

#------------------------------------------------------
find_package( Boost REQUIRED )
find_package( Protobuf REQUIRED )
find_package( ZeroMQ REQUIRED )
find_package( TinyXML REQUIRED )

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
  error.h
  console.h
  system.h
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
  manifest.h
  simulator.h
  package.h
)

# source files
set( REVEAL_CORE_SOURCES 
  console.cpp
  system.cpp
  transport_exchange.cpp
  connection.cpp
  ipc.cpp
  package.cpp
)

#------------------------------------------------------
# build the revealcore library
add_library( reveal_core SHARED ${REVEAL_CORE_HEADERS} ${REVEAL_CORE_SOURCES} )
add_dependencies( reveal_core
  reveal_messages
)

target_link_libraries( reveal_core 
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARY}
  ${ZeroMQ_LIBRARIES} 
  ${TinyXML_LIBRARIES}
)

target_link_libraries( reveal_core 
  reveal_messages
)

#------------------------------------------------------
# install
set_target_properties( reveal_core PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_core DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${REVEAL_CORE_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/core COMPONENT headers )

