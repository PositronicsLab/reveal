cmake_minimum_required(VERSION 2.8.7)

project( Reveal_DB )

#------------------------------------------------------
# find all packages
find_package( MongoDB REQUIRED )

#------------------------------------------------------
# header files
set( REVEAL_DB_HEADERS 
  query.h
  database.h
  importer.h
)

# source files
set( REVEAL_DB_SOURCES 
  query.cpp
  database.cpp
  importer.cpp
)

#------------------------------------------------------
# build the library
add_library( reveal_db SHARED ${REVEAL_DB_HEADERS} ${REVEAL_DB_SOURCES} )
add_dependencies( reveal_db reveal_core )

target_link_libraries( reveal_db 
  ${MongoDB_LIBRARIES}
  reveal_core
)

#------------------------------------------------------
# install
set_target_properties( reveal_db PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS reveal_db DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${REVEAL_DB_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/db COMPONENT headers )

#add_subdirectory( test )


