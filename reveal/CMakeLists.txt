include_directories( .. )

add_subdirectory( messages )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ) # for generated .h's
add_subdirectory( core )

if( ${INSTALL_SAMPLES_SERVER} OR ${INSTALL_ANALYTICS_SERVER} )
  add_subdirectory( db )
endif()

if( ${INSTALL_ANALYTICS_SERVER} OR ${INSTALL_ANALYTICS_CLIENT} )
  add_subdirectory( analytics )
endif()

if( ${INSTALL_SAMPLES_SERVER} OR ${INSTALL_SAMPLES_CLIENT} )
  add_subdirectory( sim )
  add_subdirectory( samples )
endif()
