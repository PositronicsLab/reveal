cmake_minimum_required(VERSION 2.8.7)

project( Reveal_DB )

#------------------------------------------------------
# find all packages
#find_package( MongoDB REQUIRED )

include_directories( 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${REVEAL_CORE_INCLUDE_DIR}
)

link_directories(
  /usr/local/lib/libmongoclient.so
)

#------------------------------------------------------
# header files
set( REVEAL_DB_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/query.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Reveal/database.h
)

# source files
set( REVEAL_DB_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/query.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/database.cpp
)

#------------------------------------------------------
# build the library
add_library( revealdb SHARED ${REVEAL_DB_HEADERS} ${REVEAL_DB_SOURCES} )
add_dependencies( revealdb revealcore )

target_link_libraries( revealdb 
  revealcore
  /usr/local/lib/libmongoclient.so
)

#------------------------------------------------------
# install
set_target_properties( revealdb PROPERTIES SOVERSION ${REVEAL_MAJOR_VERSION} VERSION ${REVEAL_VERSION_FULL} )
install( TARGETS revealdb DESTINATION ${LIB_INSTALL_DIR} COMPONENT shlib )
install( FILES ${REVEAL_DB_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT headers )

#------------------------------------------------------
# create the reveal db include and library variables for future linking
set( REVEAL_DB_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# set the variables in PARENT_SCOPE as well
set( REVEAL_DB_INCLUDE_DIR ${REVEAL_DB_INCLUDE_DIR} PARENT_SCOPE )

add_subdirectory( test )


