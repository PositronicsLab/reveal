cmake_minimum_required(VERSION 2.8.7)

set( COMPONENT_NAME "db" )
set( COMPONENT_DIR "${COMPONENT_NAME}")
project( "reveal_${COMPONENT_NAME}" )

#------------------------------------------------------
# find all packages
find_package( MongoDB REQUIRED )
find_package( Boost REQUIRED COMPONENTS system thread )

#------------------------------------------------------
# header files
set( HDRS
  service.h 
  database.h
  mongo/mongo.h
  mongo/user.h
  mongo/session.h
  mongo/digest.h
  mongo/scenario.h
  mongo/experiment.h
  mongo/trial.h
  mongo/solution.h
  mongo/solution_set.h
  mongo/analyzer.h
  mongo/analysis.h
  mongo/link.h
  mongo/joint.h
  mongo/model.h
  importer.h
)

# source files
set( SRCS 
  database.cpp
  mongo/mongo.cpp
  mongo/user.cpp
  mongo/session.cpp
  mongo/digest.cpp
  mongo/scenario.cpp
  mongo/experiment.cpp
  mongo/trial.cpp
  mongo/solution.cpp
  mongo/solution_set.cpp
  mongo/analyzer.cpp
  mongo/analysis.cpp
  mongo/link.cpp
  mongo/joint.cpp
  mongo/model.cpp
  importer.cpp
)

#------------------------------------------------------
# build the database library
add_library( ${PROJECT_NAME} SHARED ${HDRS} ${SRCS} )
add_dependencies( ${PROJECT_NAME} 
  reveal_core 
)

target_link_libraries( ${PROJECT_NAME} 
  ${MongoDB_LIBRARIES}
  ${Boost_LIBRARIES}
  reveal_core
)

#------------------------------------------------------
# install the database library
set_target_properties( ${PROJECT_NAME} PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${HDRS} DESTINATION ${INCLUDE_INSTALL_DIR}/${COMPONENT_DIR} COMPONENT headers )

#------------------------------------------------------
#------------------------------------------------------
# build the configuration tool
set( CONFIG_UTILITY_NAME "${PROJECT_NAME}_config" )

add_executable( ${CONFIG_UTILITY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/mongo/configure_db.cpp )
add_dependencies( ${CONFIG_UTILITY_NAME} 
  reveal_db 
  reveal_core 
)

target_link_libraries( ${CONFIG_UTILITY_NAME} 
  ${MongoDB_LIBRARIES}
  reveal_core
  reveal_db
)

#------------------------------------------------------
# install the configuration tool
set_target_properties( ${CONFIG_UTILITY_NAME} PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS ${CONFIG_UTILITY_NAME} DESTINATION ${BIN_INSTALL_DIR} )

#------------------------------------------------------
#------------------------------------------------------
# build tests
#if( BUILD_TESTS )
#  add_subdirectory( test )
#endif()

