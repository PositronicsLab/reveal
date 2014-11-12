cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Analytics )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( DL REQUIRED )

include_directories( 
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

link_directories(
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
)

#------------------------------------------------------
# header files
set( REVEAL_ANALYTICS_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analytics/types.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analytics/module.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analytics/plugin.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analytics/worker.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/analytics/pool.h
)

# source files
set( REVEAL_ANALYTICS_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/src/pool.cpp
)

#------------------------------------------------------
add_library( revealanalytics SHARED ${REVEAL_ANALYTICS_HEADERS} ${REVEAL_ANALYTICS_SOURCES} )
add_dependencies( revealanalytics 
  revealmessages 
  revealcore
  revealdb 
)

target_link_libraries( revealanalytics
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
  revealmessages
  revealcore
  revealdb
)

#------------------------------------------------------
# install
set_target_properties( revealanalytics PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealanalytics DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${REVEAL_ANALYTICS_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT headers )

#------------------------------------------------------

# create the reveal include variable for future linking
set( REVEAL_ANALYTICS_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# set the variable in PARENT_SCOPE as well
set( REVEAL_ANALYTICS_INCLUDE_DIR ${REVEAL_ANALYTICS_INCLUDE_DIR} PARENT_SCOPE )

# build the tests
#add_subdirectory( test )
