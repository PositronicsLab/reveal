/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

plugin.h defines the plugin_c implementation of the module_c interface.  Plugins
use the DL library to dynamically link analyzers implemented in c++ and compiled
into shared objects 
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_PLUGIN_H_
#define _REVEAL_ANALYTICS_PLUGIN_H_

//-----------------------------------------------------------------------------

#include <dlfcn.h>              //POSIX

#include "Reveal/core/pointers.h"
#include "Reveal/core/solution_set.h"
#include "Reveal/analytics/types.h"
#include "Reveal/core/analysis.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

class plugin_c : public Reveal::Analytics::module_c {
private:
  analyzer_f analyzer;   /// pointer to the plugins analyzer implementation
  void* HANDLE; 	 /// pointer to the plugin dynamic library handle

public:
  //---------------------------------------------------------------------------
  /// Default constructor
  plugin_c( void ) { HANDLE = NULL; }
  //---------------------------------------------------------------------------
  /// Destructor
  virtual ~plugin_c( void ) { if( HANDLE ) close(); }

  //---------------------------------------------------------------------------
  /// Loads a plugin from a path.  Fulfills the abstract module interface
  /// @param path the path to the plugin
  /// @return if load in successful returns ERROR_NONE otherwise returns an 
  ///         enumerated value describing the error
  virtual error_e load( std::string path ) {
    return read( path.c_str() );
  }

  //---------------------------------------------------------------------------
  /// Executes the analyze function of the loaded plugin.  Fulfills the 
  /// abstract module interface
  /// @param in the solution set submitted for analysis
  /// @param out the result of the module's analytics computation
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  virtual error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) {
    assert( HANDLE );
    return analyzer( in, out );
  }
  
private:
  //---------------------------------------------------------------------------
  /// connects the plugin to the system and validates the plugin interface
  /// @param path the path to the plugin to read
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  error_e read( const char* path ) {

    // open the plugin
    open( path );

    /// if the handle is null, open failed so report and return error
    if( !HANDLE ) {
      std::cerr << " failed to read plugin from " << path << std::endl;
      std::cerr << "  " << dlerror( ) << std::endl;
      return ERROR_OPEN;
    }

    // locate the analyzer function
    analyzer = (analyzer_f) dlsym(HANDLE, "analyze");
    const char* dlsym_error = dlerror( );

    // if the analyzer function does not match the signiture or cannot be 
    // located report and return error
    if( dlsym_error ) {
      std::cerr << " warning: cannot load symbol 'analyze' from " << path << std::endl;
      std::cerr << "        error follows: " << std::endl << dlsym_error << std::endl;
      close();
      return ERROR_LINK;
    }

    // otherwise the plugin is loaded so return success
    return ERROR_NONE;
  }

  //---------------------------------------------------------------------------
  /// opens the plugin as a dynamic library
  /// @param path the path to the plugin to read
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  error_e open( const char* path ) {
    HANDLE = dlopen( path, RTLD_LAZY );
    if( !HANDLE ) return ERROR_OPEN;
    return ERROR_NONE;
  }

  //---------------------------------------------------------------------------
  /// closes an open plugin
  void close( void ) {
    if( HANDLE ) dlclose( HANDLE );
    HANDLE = NULL;
  }

};

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_PLUGIN_H_
