# For Debug Build :: -DCMAKE_BUILD_TYPE=Debug

#if(COMMAND CMAKE_POLICY)
#  CMAKE_POLICY(SET CMP0003 NEW)
#  CMAKE_POLICY(SET CMP0004 NEW)
#endif(COMMAND CMAKE_POLICY)

project( Reveal )
cmake_minimum_required( VERSION 2.8.7 )

string( TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER )
string( TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER )

set( REVEAL_VERSION_NAME "air" )
set( REVEAL_MAJOR_VERSION 0 )
set( REVEAL_MINOR_VERSION 1 )
set( REVEAL_PATCH_VERSION 0 )

set( REVEAL_VERSION ${REVEAL_MAJOR_VERSION}.${REVEAL_MINOR_VERSION} )
set( REVEAL_VERSION_FULL ${REVEAL_MAJOR_VERSION}.${REVEAL_MINOR_VERSION}.${REVEAL_PATCH_VERSION} )

message (STATUS "${PROJECT_NAME} version ${REVEAL_VERSION_FULL}")

set( REVEAL_CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake_modules CACHE PATH "CMake scripts" )
set( CMAKE_MODULE_PATH ${REVEAL_CMAKE_DIR} ${CMAKE_MODULE_PATH} )

#------------------------------------------------------
include( ${REVEAL_CMAKE_DIR}/reveal_cpack.cmake )
set( CPACK_PACKAGE_VERSION "${REVEAL_VERSION_FULL}" )
set( CPACK_PACKAGE_VERSION_MAJOR "${REVEAL_MAJOR_VERSION}" )
set( CPACK_PACKAGE_VERSION_MINOR "${REVEAL_MINOR_VERSION}" )
set( CPACK_PACKAGE_VERSION_PATCH "${REVEAL_PATCH_VERSION}" )

if( CPACK_GENERATOR )
  message( STATUS "Found CPack generators: ${CPACK_GENERATOR}" )

  configure_file( "${REVEAL_CMAKE_DIR}/cpack_options.cmake.in" ${REVEAL_CPACK_CFG_FILE} @ONLY )
  set( CPACK_PROJECT_CONFIG_FILE ${REVEAL_CPACK_CFG_FILE} )
  include( CPack )
endif()

if(UNIX AND NOT WIN32)
  set (CMAKE_INSTALL_PREFIX "/usr" CACHE STRING "Install Prefix")
 find_program(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
 if(CMAKE_UNAME)
   exec_program(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
   set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
     "processor type (i386 and x86_64)")
   if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
     ADD_DEFINITIONS(-fPIC)
   endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
 endif(CMAKE_UNAME)
endif()

set (CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# developer's option to cache PKG_CONFIG_PATH and
# LD_LIBRARY_PATH for local installs
if(PKG_CONFIG_PATH)
  set (ENV{PKG_CONFIG_PATH} ${PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH})
endif()
if(LD_LIBRARY_PATH)
  set (ENV{LD_LIBRARY_PATH} ${LD_LIBRARY_PATH}:$ENV{LD_LIBRARY_PATH})
endif()

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR include)
endif(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
if(NOT DEFINED CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR bin)
endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

set( INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/Reveal" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" )
set( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}" )

set( MIN_PROTOBUF_VERSION 2.4.1 CACHE INTERNAL "Protocol Buffers min version requirement" FORCE )
set( MIN_ZMQ_VERSION 4.0.3 CACHE INTERNAL "ZeroMQ min version requirement" FORCE )
set( MIN_BOOST_VERSION 1.46.1 CACHE INTERNAL "Boost min version requirement" FORCE )

#------------------------------------------------------
MESSAGE(STATUS "Checking reveal build type")
# Set the default build type
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo Profile Check" FORCE)
endif (NOT CMAKE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPERCASE)

#------------------------------------------------------

#SET(CMAKE_CXX_COMPILER clang)
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -g ")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g ")
#SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -Bsymbolic")
#SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--warn-unresolved-symbols -Bsymbolic")
#SET(CMAKE_SHARED_LINKER_FLAGS "-Bsymbolic --verbose")
#SET(CMAKE_SHARED_LINKER_FLAGS "-Bsymbolic -E")
#SET(CMAKE_SHARED_LINKER_FLAGS "-z defs")


set( REVEAL_MESSAGES_INCLUDE_DIR )
set( REVEAL_MESSAGES_LIBRARY )
set( REVEAL_CORE_INCLUDE_DIR )
set( REVEAL_CORE_LIBRARY )
set( REVEAL_CORE_LIBRARIES )
set( REVEAL_ANALYTICS_INCLUDE_DIR )
set( REVEAL_ANALYTICS_LIBRARY )
set( REVEAL_ANALYTICS_LIBRARIES )
set( REVEAL_DB_INCLUDE_DIR )
set( REVEAL_DB_LIBRARY )
set( REVEAL_DB_LIBRARIES )
set( REVEAL_CLIENT_INCLUDE_DIR )
set( REVEAL_CLIENT_SOURCES )

add_subdirectory( core )
add_subdirectory( db )
add_subdirectory( analytics )
add_subdirectory( server )
add_subdirectory( client )
#add_subdirectory( scenarios )

#------------------------------------------------------
# Make the cmake config files
set( PKG_NAME ${PROJECT_NAME_UPPER} )
set( PKG_LIBRARIES
  revealmessages
  revealcore
  revealdb
  revealanalytics )

set(PKG_DEPENDS Boost Protobuf ZeroMQ)

set(cmake_conf_file "cmake_modules/reveal-config.cmake")
set(cmake_conf_version_file "cmake_modules/reveal-config-version.cmake")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${cmake_conf_file}.in" "${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_file}" @ONLY)
# Use write_basic_package_version_file to generate a ConfigVersion file that
# allow users of gazebo to specify the API or version to depend on
# TODO: keep this instruction until deprecate Ubuntu/Precise and update with
# https://github.com/Kitware/CMake/blob/v2.8.8/Modules/CMakePackageConfigHelpers.cmake
include( WriteBasicConfigVersionFile )
write_basic_config_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_version_file}
    VERSION "${REVEAL_VERSION_FULL}"
    COMPATIBILITY SameMajorVersion)
install( FILES
          ${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_file}
          ${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_version_file}
        DESTINATION
          ${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME_LOWER}/
        COMPONENT cmake )

#message( "INCLUDE_INSTALL_DIR: ${INCLUDE_INSTALL_DIR}" )
#message( "LIB_INSTALL_DIR: ${LIB_INSTALL_DIR}" )
#message( "BIN_INSTALL_DIR: ${BIN_INSTALL_DIR}" )


#------------------------------------------------------

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)




