find_package( Reveal REQUIRED )
include_directories( ${REVEAL_INCLUDE_DIRS} )
link_directories( ${REVEAL_LIBRARY_DIRS} )

add_executable( test-uri uri.cpp )
target_link_libraries( test-uri reveal_core )

add_executable( test-xml xml.cpp )
target_link_libraries( test-xml reveal_core )


