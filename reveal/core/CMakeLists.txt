cmake_minimum_required(VERSION 2.8.7)

set( COMPONENT_NAME "core" )
set( COMPONENT_DIR "${COMPONENT_NAME}")
project( "reveal_${COMPONENT_NAME}" )

#------------------------------------------------------
add_definitions( -DMAKE_BIN="${CMAKE_MAKE_PROGRAM}")
add_definitions( -DCMAKE_BIN="${CMAKE_COMMAND}")

#------------------------------------------------------
find_package( Boost REQUIRED COMPONENTS system filesystem )
find_package( ZeroMQ REQUIRED )
find_package( TinyXML REQUIRED )
find_package( DL REQUIRED )
if (UNIX)
  find_package( Threads REQUIRED )
endif()

include_directories(
  ${DL_INCLUDE_DIRS}
  ${ZeroMQ_INCLUDE_DIRS}
)

link_directories(
  ${DL_LIBRARIES}
  ${ZeroMQ_LIBRARIES}
)

#------------------------------------------------------
# header files
set( HDRS
  error.h
  console.h
  system.h
  pointers.h
  user.h
  session.h
  authorization.h
  exchange.h
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
  solution_set.h
  analyzer.h
  analysis.h
  log.h
  manifest.h
  simulator.h
  package.h
  xml.h
  exporter.h
  datamap.h
  component.h
  serial.h
  app.h
)

# source files
set( SRCS
  console.cpp
  system.cpp
  connection.cpp
  ipc.cpp
  simulator.cpp
  package.cpp
  xml.cpp
  exporter.cpp
  datamap.cpp
  app.cpp
)

set( LIBS
  ${DL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${ZeroMQ_LIBRARIES}
  ${TinyXML_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  reveal_messages
)

add_subdirectory( test )

#------------------------------------------------------
# build the revealcore library
add_library( ${PROJECT_NAME} SHARED ${HDRS} ${SRCS} )
add_dependencies( ${PROJECT_NAME}
  reveal_messages
)

target_link_libraries( ${PROJECT_NAME} ${LIBS} )

#------------------------------------------------------
# install
set_target_properties( ${PROJECT_NAME} PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_DIR} COMPONENT headers )

#------------------------------------------------------
# unit testing
enable_testing()
add_executable( system_TEST system_TEST.cpp ${HDRS} ${SRCS} )
target_link_libraries( system_TEST ${LIBS} )

add_test( NAME system COMMAND system_TEST system_TEST )
add_custom_command( TARGET system_TEST POST_BUILD COMMAND ctest system_TEST )

