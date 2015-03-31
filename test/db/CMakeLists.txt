find_package( Reveal REQUIRED )
include_directories( ${REVEAL_INCLUDE_DIRS} )
link_directories( ${REVEAL_LIBRARY_DIRS} )

add_executable( test-db-importer importer.cpp )
target_link_libraries( test-db-importer reveal_core reveal_db )

