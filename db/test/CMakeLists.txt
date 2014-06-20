cmake_minimum_required(VERSION 2.8.7)

project( Reveal_DB_tests )

# additional include paths
set( REVEAL_DB_TEST_INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
)

# any libraries
set( REVEAL_DB_TEST_LIBRARIES
  ${REVEAL_CORE_LIBRARY}
  ${REVEAL_CORE_LIBRARIES}
  ${REVEAL_DB_LIBRARY}
  ${REVEAL_DB_LIBRARIES}
)

# add the include paths
include_directories( ${REVEAL_DB_TEST_INCLUDE_DIRS} )

# build the executables
add_executable( test-reveal-db-insert-scenario ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_db_insert_scenario.cpp )
add_dependencies( test-reveal-db-insert-scenario ${REVEAL_CORE_LIBRARY} ${REVEAL_DB_LIBRARY} )
target_link_libraries( test-reveal-db-insert-scenario LINK_PRIVATE ${REVEAL_DB_TEST_LIBRARIES} )

##disabled
#add_executable( test-reveal-db-fetch ${CMAKE_CURRENT_SOURCE_DIR}/db/test/test_reveal_db_fetch.cpp ${REVEAL_DB_HEADERS} ${REVEAL_DB_SOURCES} )
#target_link_libraries( test-reveal-db-fetch ${REVEAL_DB_LIBRARIES} ${REVEAL_DB_LIBRARY} )

