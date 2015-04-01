/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

package.h defines the package_c class that encapsulates all components necessary
to execute a particular scenario in a given simulator.  package_c has facilities
to configure and make plugins necessary for the simulator to integrate into
Reveal.
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_PACKAGE_H_
#define _REVEAL_CORE_PACKAGE_H_
//-----------------------------------------------------------------------------

#include <string>
#include <boost/shared_ptr.hpp>

#include "Reveal/core/system.h"
#include "Reveal/core/manifest.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

/** 
Defines a signal handler used by the package_c class to detect exit condition when building a package.
*/
class child_exit_handler_c : public sighandler_c {
protected:
  /// Flag indicating whether the signalhandler has been called.
  static bool _quit;

public:
  /// Default constructor
  child_exit_handler_c( void ); 

  /// Destructor
  ~child_exit_handler_c( void );

  /// The signal handler callback function
  static void handler( int signum );

  /// Gets the value of the quit flag
  /// @return true if the signalhandler detected child process exit otherwise true
  static bool quit( void ); 

  /// sighandler_c interface.  Installs the signal handler into a system
  virtual void install( void );

  /// sighandler_c interface.  Uninstalls the signal handler from a system
  virtual void uninstall( void );
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class package_c;
typedef boost::shared_ptr<package_c> package_ptr;
//-----------------------------------------------------------------------------
/**
Encapsulates all components necessary to execute a particular scenario in a given simulator.  Has facilities to configure and make plugins necessary for the simulator to integrate into Reveal.
*/
class package_c {
public:
  /// Constructor
  package_c( std::string source_path, std::string build_path );
  /// Destructor
  virtual ~package_c( void );

  /// Read the package manifest
  /// @param sim_key The element key name of the simulator to search for
  /// @return a valid xml_element_ptr OR an empty pointer
  xml_element_ptr read( std::string sim_key );

  /// Configure the package before building
  /// @return true if cmake successfully completed OR false if cmake failed.
  bool configure( void );

  /// Build the package
  /// @param build_products the set of all build products that are produced by
  ///        the build process
  /// @return true if make successfully built the build products OR false if
  ///        the build products were not generated
  bool make( std::vector<std::string> build_products );

protected:
  /// The path to the package's source directory
  std::string _source_path;
  /// The path to the package's build directory
  std::string _build_path;

  /// The CMake worker thread function
  /// @param args args[0]=<source_path> and args[1]=<build_path>
  /// @return true if cmake successfully completed OR false if cmake failed.
  static void* cmake_worker( void* args );

  /// The Make worker thread function
  /// @param args args[0]=<build_path> 
  /// @return true if make successfully built the build products OR false if
  ///        the build products were not generated
  static void* make_worker( void* args );

  /// Configuration function that accepts a structured parameter list where
  /// @param args args[0]=<source_path> and args[1]=<build_path>
  /// @return true if cmake successfully completed OR false if cmake failed.
  bool cmake_package( char* const* args );

  /// Configuration function that accepts source and build paths
  /// @param source_path path to the source directory
  /// @param source_path path to the build directory
  /// @return true if cmake successfully completed OR false if cmake failed.
  bool cmake_package( std::string source_path, std::string build_path );

  /// Build function that accepts a structured parameter list where 
  /// @param args args[0]=<source_path>, args[1]=<n build_products>, 
  ///        args[2..n+2]<build_product>
  /// @return true if make successfully built the build products OR false if
  ///        the build products were not generated
  bool make_package( char* const* args );

  /// Build function that accepts source path and set of build products
  /// @param args 
  /// @return true if make successfully built the build products OR false if
  ///        the build products were not generated
  bool make_package( std::string build_path, std::vector<std::string> build_products );

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_PACKAGE_H_
