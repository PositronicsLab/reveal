cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Client )

#------------------------------------------------------
find_package( Pthread REQUIRED )
find_package( Boost REQUIRED )

#------------------------------------------------------
# common header files
set( REVEAL_CLIENT_HEADERS 
  client.h
)

# common source files
set( REVEAL_CLIENT_SOURCES 
  client.cpp
)

#------------------------------------------------------
add_executable( reveal_client ${CMAKE_CURRENT_SOURCE_DIR}/reveal_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )

add_dependencies( reveal_client 
  reveal_core
  reveal_sim_gazebo
)
target_link_libraries( reveal_client 
  ${PTHREAD_LIBRARIES} 
  reveal_core
  reveal_sim_gazebo 
)

#------------------------------------------------------
set_target_properties( reveal_client PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_client DESTINATION ${BIN_INSTALL_DIR} )
install( FILES ${REVEAL_CLIENT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/client COMPONENT headers )

#------------------------------------------------------
#------------------------------------------------------
# Note: this is only valid if the db is local.  May belong in a different 
# package as we don't want to have any dependency to database in general client
# build tree
#------------------------------------------------------
## header files
#set( REVEAL_IMPORT_HEADERS 
#  importer.h
#)

## source files
#set( REVEAL_IMPORT_SOURCES 
#  importer.cpp
#)

#add_executable( reveal_import ${CMAKE_CURRENT_SOURCE_DIR}/reveal_import.cpp ${REVEAL_IMPORT_HEADERS} ${REVEAL_IMPORT_SOURCES} )

#add_dependencies( reveal_import 
#  reveal_core
#  reveal_db
#)
#target_link_libraries( reveal_import
##  ${TinyXML_LIBRARIES}
##  ${PTHREAD_LIBRARIES} 
#  reveal_core 
#  reveal_db
#)

#------------------------------------------------------
#set_target_properties( reveal_import PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
#install( TARGETS reveal_import DESTINATION ${BIN_INSTALL_DIR} )
##install( FILES ${REVEAL_CLIENT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/client COMPONENT headers )

## build the tests
##add_subdirectory( test )

