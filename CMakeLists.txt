# For Debug Build :: -DCMAKE_BUILD_TYPE=Debug

cmake_minimum_required( VERSION 2.8.7 )

#if(COMMAND CMAKE_POLICY)
#  CMAKE_POLICY(SET CMP0003 NEW)
#  CMAKE_POLICY(SET CMP0004 NEW)
#endif(COMMAND CMAKE_POLICY)

set( COMPONENT_NAME "" )
set( COMPONENT_DIR "${COMPONENT_NAME}")
project( "reveal" )

set( CMAKE_MODULES_DIR ${PROJECT_SOURCE_DIR}/cmake CACHE PATH "CMake scripts" )
set( CMAKE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/cmake CACHE PATH "CMake scripts" )
set( CMAKE_MODULE_PATH ${CMAKE_MODULES_DIR} ${CMAKE_MODULE_PATH} )

option( INSTALL_SAMPLES_CLIENT "Install the Reveal Samples client" ON )
option( INSTALL_SAMPLES_SERVER "Install the Reveal Samples server" ON )
option( INSTALL_ANALYTICS_CLIENT "Install the Reveal Analytics client" ON )
option( INSTALL_ANALYTICS_SERVER "Install the Reveal Analytics server" ON )

#option( BUILD_TESTS "compile test programs" OFF )

string( TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER )
string( TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER )

set( REVEAL_VERSION_NAME "air" )
set( REVEAL_MAJOR_VERSION 0 )
set( REVEAL_MINOR_VERSION 3 )
set( REVEAL_PATCH_VERSION 0 )

set( REVEAL_VERSION ${REVEAL_MAJOR_VERSION}.${REVEAL_MINOR_VERSION} )
set( REVEAL_VERSION_FULL ${REVEAL_MAJOR_VERSION}.${REVEAL_MINOR_VERSION}.${REVEAL_PATCH_VERSION} )

#message (STATUS "${PROJECT_NAME} version ${REVEAL_VERSION_FULL}")

#------------------------------------------------------
include( ${CMAKE_MODULES_DIR}/reveal_cpack.cmake )
set( CPACK_PACKAGE_VERSION "${REVEAL_VERSION_FULL}" )
set( CPACK_PACKAGE_VERSION_MAJOR "${REVEAL_MAJOR_VERSION}" )
set( CPACK_PACKAGE_VERSION_MINOR "${REVEAL_MINOR_VERSION}" )
set( CPACK_PACKAGE_VERSION_PATCH "${REVEAL_PATCH_VERSION}" )

if( CPACK_GENERATOR )
  message( STATUS "Found CPack generators: ${CPACK_GENERATOR}" )

  configure_file( "${CMAKE_MODULES_DIR}/cpack_options.cmake.in" ${REVEAL_CPACK_CFG_FILE} @ONLY )
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

#unsure as to why these are necessary when they are not in other frameworks
#perhaps it is policies
if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR include)
endif(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
if(NOT DEFINED CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR bin)
endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

set( INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" )
set( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}" )

set( MIN_PROTOBUF_VERSION 2.4.1 CACHE INTERNAL "Protocol Buffers min version requirement" FORCE )
set( MIN_ZMQ_VERSION 4.0.3 CACHE INTERNAL "ZeroMQ min version requirement" FORCE )
set( MIN_BOOST_VERSION 1.46.1 CACHE INTERNAL "Boost min version requirement" FORCE )

#------------------------------------------------------
set( DEFAULT_SAMPLES_HOST "localhost" CACHE STRING "The samples server host" )
set( DEFAULT_SAMPLES_PORT "20700" CACHE STRING "The samples server port" )
set( DEFAULT_ANALYTICS_HOST "localhost" CACHE STRING "The analytics server host" )
set( DEFAULT_ANALYTICS_PORT "20701" CACHE STRING "The analytics server port" )
set( DEFAULT_DATABASE_HOST "localhost" CACHE STRING "The database server host" )
set( DEFAULT_DATABASE_PORT "27017" CACHE STRING "The database server port" )
set( DEFAULT_PACKAGES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/reveal_packages" CACHE PATH "The path to reveal packages" )
set( DEFAULT_MONITOR_PORT "20702" CACHE STRING "The benchmark monitor ipc port" )
set( DEFAULT_SAMPLES_DATABASE_NAME "reveal_samples" CACHE STRING "The name of the samples database" )
set( DEFAULT_ANALYTICS_DATABASE_NAME "reveal_analytics" CACHE STRING "The name of the analytics database" )
set( DEFAULT_SIMS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/reveal_sims" CACHE PATH "The path to reveal sims" )
set( DEFAULT_BIN_SEARCH_PATH "/usr/local/bin:/usr/bin" CACHE PATH "The set of paths in which to search for simulator binaries" )
set( DEFAULT_LIB_SEARCH_PATH "/usr/local/lib:/usr/lib" CACHE PATH "The set of paths in which to search for simulator libraries" )

add_definitions( -DDEFAULT_SAMPLES_HOST=${DEFAULT_SAMPLES_HOST} )
add_definitions( -DDEFAULT_SAMPLES_PORT=${DEFAULT_SAMPLES_PORT} )
add_definitions( -DDEFAULT_ANALYTICS_HOST=${DEFAULT_ANALYTICS_HOST} )
add_definitions( -DDEFAULT_ANALYTICS_PORT=${DEFAULT_ANALYTICS_PORT} )
add_definitions( -DDEFAULT_DATABASE_HOST=${DEFAULT_DATABASE_HOST} )
add_definitions( -DDEFAULT_DATABASE_PORT=${DEFAULT_DATABASE_PORT} )
add_definitions( -DDEFAULT_PACKAGES_PATH=${DEFAULT_PACKAGES_PATH} )
add_definitions( -DDEFAULT_MONITOR_PORT=${DEFAULT_MONITOR_PORT} )
add_definitions( -DDEFAULT_SAMPLES_DATABASE_NAME=${DEFAULT_SAMPLES_DATABASE_NAME} )
add_definitions( -DDEFAULT_ANALYTICS_DATABASE_NAME=${DEFAULT_ANALYTICS_DATABASE_NAME} )
add_definitions( -DDEFAULT_SIMS_PATH=${DEFAULT_SIMS_PATH} )

#------------------------------------------------------
file( WRITE ${PROJECT_BINARY_DIR}/setup.sh.in "" )

#if database needed
if( ${INSTALL_SAMPLES_SERVER} OR ${INSTALL_ANALYTICS_SERVER} )
  file( READ ${CMAKE_MODULES_DIR}/setup.sh.in.db INSTR )
  file( APPEND ${PROJECT_BINARY_DIR}/setup.sh.in "${INSTR}" )
endif()

#if samples installed
if( ${INSTALL_SAMPLES_SERVER} OR ${INSTALL_SAMPLES_CLIENT} )
  file( READ ${CMAKE_MODULES_DIR}/setup.sh.in.samples INSTR )
  file( APPEND ${PROJECT_BINARY_DIR}/setup.sh.in "${INSTR}" )
endif()

#if analytics installed
if( ${INSTALL_ANALYTICS_SERVER} OR ${INSTALL_ANALYTICS_CLIENT} )
  file( READ ${CMAKE_MODULES_DIR}/setup.sh.in.analytics INSTR )
  file( APPEND ${PROJECT_BINARY_DIR}/setup.sh.in "${INSTR}" )
endif()

#if packages required
if( ${INSTALL_ANALYTICS_CLIENT} OR ${INSTALL_SAMPLES_CLIENT} )
  file( READ ${CMAKE_MODULES_DIR}/setup.sh.in.packages INSTR )
  file( APPEND ${PROJECT_BINARY_DIR}/setup.sh.in "${INSTR}" )
endif()

#if sims required
if( ${INSTALL_SAMPLES_CLIENT} )
  file( READ ${CMAKE_MODULES_DIR}/setup.sh.in.sims INSTR )
  file( APPEND ${PROJECT_BINARY_DIR}/setup.sh.in "${INSTR}" )
endif()

configure_file( ${PROJECT_BINARY_DIR}/setup.sh.in ${PROJECT_BINARY_DIR}/setup.sh @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/setup.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/share/reveal-${REVEAL_VERSION}/)
# Also install the setup.sh in an unversioned location
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/setup.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/share/reveal/)


#------------------------------------------------------
MESSAGE(STATUS "Checking Reveal build type")
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
#SET(CMAKE_CXX_COMPILER "/usr/bin/gcc-4.8" )
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g" )
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g")

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory( reveal )

#if( BUILD_TESTS )
#  add_subdirectory( test )
#endif()

#------------------------------------------------------
# Make the cmake config files
set( PKG_NAME ${PROJECT_NAME_UPPER} )
set( PKG_LIBRARIES
  reveal_messages
  reveal_core
)

if( ${INSTALL_REVEAL_SERVER} )
  set( PKG_LIBRARIES
    ${PKG_LIBRARIES}
    reveal_db
    reveal_analytics
  )
endif( ${INSTALL_REVEAL_SERVER} )
set(PKG_DEPENDS Boost Protobuf ZeroMQ)

set(cmake_conf_file "${CMAKE_MODULES_DIR}/reveal-config.cmake")
set(cmake_conf_version_file "${CMAKE_MODULES_DIR}/reveal-config-version.cmake")
configure_file("${CMAKE_MODULE_DIR}/${cmake_conf_file}.in" "${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_file}" @ONLY)
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
########### Add uninstall target ###############
configure_file(
  "${CMAKE_MODULES_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_MODULES_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_OUTPUT_DIR}/cmake_uninstall.cmake")


#------------------------------------------------------

#SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#SET(CMAKE_SKIP_BUILD_RPATH  FALSE)




