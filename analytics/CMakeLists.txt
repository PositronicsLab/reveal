cmake_minimum_required(VERSION 2.8.7)

project( Reveal_Analytics )

find_package( Pthread REQUIRED )
find_package( DL REQUIRED )

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

# additional include paths
set( REVEAL_ANALYTICS_INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# any libraries
set( REVEAL_ANALYTICS_LIBRARIES
  ${PTHREAD_LIBRARIES}
  ${DL_LIBRARIES}
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
  ${REVEAL_DB_LIBRARY}
  ${REVEAL_DB_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_ANALYTICS_INCLUDE_DIRS} )

# build the executables
# build the revealcore library
add_library( revealanalytics SHARED ${REVEAL_ANALYTICS_HEADERS} ${REVEAL_ANALYTICS_SOURCES} )
add_dependencies( revealanalytics revealmessages )
#add_dependencies( revealcore ${REVEAL_MESSAGES_LIBRARY} )
#target_link_libraries( revealcore LINK_PRIVATE ${REVEAL_CORE_LIBARIES})
target_link_libraries(revealanalytics LINK_PRIVATE ${ZeroMQ_LIBRARIES})
target_link_libraries(revealanalytics LINK_PRIVATE ${PROTOBUF_LIBRARIES})
target_link_libraries(revealanalytics LINK_PRIVATE ${REVEAL_MESSAGES_LIBRARY})
target_link_libraries(revealanalytics LINK_PRIVATE ${REVEAL_ANALYTICS_LIBRARIES})

# create the reveal core include and library variables for future linking
set( REVEAL_ANALYTICS_INCLUDE_DIR
#  ${CMAKE_CURRENT_BINARY_DIR}   #for proto headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set( REVEAL_ANALYTICS_LIBRARY
  ${LIBRARY_OUTPUT_PATH}/librevealanalytics.so
)

# set the variables in PARENT_SCOPE as well
set( REVEAL_ANALYTICS_LIBRARIES ${REVEAL_ANALYTICS_LIBRARIES} PARENT_SCOPE )
set( REVEAL_ANALYTICS_INCLUDE_DIR ${REVEAL_ANALYTICS_INCLUDE_DIR} PARENT_SCOPE )
set( REVEAL_ANALYTICS_LIBRARY ${REVEAL_ANALYTICS_LIBRARY} PARENT_SCOPE )

# build the tests
#add_subdirectory( test )
