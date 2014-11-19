cmake_minimum_required(VERSION 2.8.7)

project( Reveal_DB_tests )

# additional include paths
set( REVEAL_DB_TEST_INCLUDE_DIRS
  ${REVEAL_CORE_INCLUDE_DIR}
  ${REVEAL_DB_INCLUDE_DIR}
)

# any libraries
set( REVEAL_DB_TEST_LIBRARIES
  revealcore
  revealdb
)

# add the include paths
include_directories( ${REVEAL_DB_TEST_INCLUDE_DIRS} )

# build the executables
add_executable( test-reveal-db-insert-scenario ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_db_insert_scenario.cpp )
add_dependencies( test-reveal-db-insert-scenario 
  revealcore 
  revealdb
)
target_link_libraries( test-reveal-db-insert-scenario 
  revealcore
  revealdb
)

add_executable( test-reveal-db-fetch ${CMAKE_CURRENT_SOURCE_DIR}/test_reveal_db_fetch.cpp )
add_dependencies( test-reveal-db-fetch 
  revealcore 
  revealdb
)
target_link_libraries( test-reveal-db-fetch 
  revealcore
  revealdb
)

