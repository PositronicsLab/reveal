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
add_executable( revealclient ${CMAKE_CURRENT_SOURCE_DIR}/reveal_client.cpp ${REVEAL_CLIENT_HEADERS} ${REVEAL_CLIENT_SOURCES} )

add_dependencies( revealclient 
  revealcore
  revealsimgazebo
)
target_link_libraries( revealclient 
  ${PTHREAD_LIBRARIES} 
  revealcore
  revealsimgazebo 
)

#------------------------------------------------------
set_target_properties( revealclient PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealclient DESTINATION ${BIN_INSTALL_DIR} )
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

#add_executable( revealimport ${CMAKE_CURRENT_SOURCE_DIR}/reveal_import.cpp ${REVEAL_IMPORT_HEADERS} ${REVEAL_IMPORT_SOURCES} )

#add_dependencies( revealimport 
#  revealcore
#  revealdb
#)
#target_link_libraries( revealimport
##  ${TinyXML_LIBRARIES}
##  ${PTHREAD_LIBRARIES} 
#  revealcore 
#  revealdb
#)

#------------------------------------------------------
#set_target_properties( revealimport PROPERTIES VERSION ${REVEAL_VERSION_FULL} )
#install( TARGETS revealimport DESTINATION ${BIN_INSTALL_DIR} )
##install( FILES ${REVEAL_CLIENT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/client COMPONENT headers )

## build the tests
##add_subdirectory( test )

