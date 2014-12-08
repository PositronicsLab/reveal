include_directories( .. )

add_subdirectory( messages )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. )

add_subdirectory( core )
add_subdirectory( db )
add_subdirectory( analytics )
add_subdirectory( server )
add_subdirectory( client )
