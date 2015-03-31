include_directories( .. )
 
add_subdirectory( core )

if( ${INSTALL_REVEAL_SERVER} )
  add_subdirectory( db )
endif( ${INSTALL_REVEAL_SERVER} )

#add_subdirectory( messages )
#include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ) # for generated .h's
#add_subdirectory( core )
#add_subdirectory( sim )

#if( ${INSTALL_REVEAL_SERVER} )
#  add_subdirectory( db )
#  add_subdirectory( analytics )
#  add_subdirectory( server )
#endif( ${INSTALL_REVEAL_SERVER} )

#if( ${INSTALL_REVEAL_CLIENT} )
#  add_subdirectory( client )
#endif( ${INSTALL_REVEAL_CLIENT} )

