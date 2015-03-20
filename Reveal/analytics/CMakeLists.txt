cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Analytics )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( DL REQUIRED )

link_directories(
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
)

#------------------------------------------------------
# header files
set( REVEAL_ANALYTICS_HEADERS 
  types.h
  module.h
  plugin.h
  worker.h
#  pool.h
)

# source files
set( REVEAL_ANALYTICS_SOURCES 
  worker.cpp
#  pool.cpp
)

#------------------------------------------------------
add_library( reveal_analytics SHARED ${REVEAL_ANALYTICS_HEADERS} ${REVEAL_ANALYTICS_SOURCES} )
add_dependencies( reveal_analytics 
  reveal_messages 
  reveal_core
  reveal_db 
)

target_link_libraries( reveal_analytics
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
  reveal_messages
  reveal_core
  reveal_db
)

#------------------------------------------------------
# install
set_target_properties( reveal_analytics PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_analytics DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${REVEAL_ANALYTICS_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/analytics COMPONENT headers )

#------------------------------------------------------

# build the tests
#add_subdirectory( test )
