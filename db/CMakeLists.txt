cmake_minimum_required(VERSION 2.8.7)

project( Reveal_DB )

#---------------Reveal DB-------------------
# find all packages
#find_package( MongoDB REQUIRED )

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

# additional include paths
set( REVEAL_DB_INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# any libraries
set( REVEAL_DB_LIBRARIES
  /usr/local/lib/libmongoclient.so    #TODO: needs to be generalized
#  ${MongoDB_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_DB_INCLUDE_DIRS} )

# build the library
add_library( revealdb SHARED ${REVEAL_DB_HEADERS} ${REVEAL_DB_SOURCES} )
add_dependencies( revealdb revealcore )
target_link_libraries( revealdb LINK_PRIVATE ${REVEAL_DB_LIBARIES} ${REVEAL_CORE_LIBRARY})

# create the reveal db include and library variables for future linking
set( REVEAL_DB_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set( REVEAL_DB_LIBRARY
  ${LIBRARY_OUTPUT_PATH}/librevealdb.so
)

# set the variables in PARENT_SCOPE as well
set( REVEAL_DB_LIBRARIES ${REVEAL_DB_LIBRARIES} PARENT_SCOPE )
set( REVEAL_DB_INCLUDE_DIR ${REVEAL_DB_INCLUDE_DIR} PARENT_SCOPE )
set( REVEAL_DB_LIBRARY ${REVEAL_DB_LIBRARY} PARENT_SCOPE )

add_subdirectory( test )


