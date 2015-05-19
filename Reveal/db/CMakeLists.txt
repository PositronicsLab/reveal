cmake_minimum_required(VERSION 2.8.7)

project( Reveal_DB )

#------------------------------------------------------
# find all packages
find_package( MongoDB REQUIRED )
find_package( Boost REQUIRED COMPONENTS system thread )

#------------------------------------------------------
# header files
set( REVEAL_DB_HEADERS
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
set( REVEAL_DB_SOURCES 
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
# build the library
add_library( reveal_db SHARED ${REVEAL_DB_HEADERS} ${REVEAL_DB_SOURCES} )
add_dependencies( reveal_db reveal_core )

target_link_libraries( reveal_db 
  ${MongoDB_LIBRARIES}
  ${Boost_LIBRARIES}
  reveal_core
)

#------------------------------------------------------
# install
set_target_properties( reveal_db PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_db DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${REVEAL_DB_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/db COMPONENT headers )

if( BUILD_TESTS )
  add_subdirectory( test )
endif()

