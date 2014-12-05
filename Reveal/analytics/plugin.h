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
public:
  plugin_c( void ) { HANDLE = NULL; }
  virtual ~plugin_c( void ) {}

  virtual error_e load( std::string filename ) {
    return read( filename.c_str() );
  }

  // TODO : Refactor signature.  The input is a solution set but the output is 
  // an analysis
  virtual error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) {
    return analyzer( in, out );
  }
  

private:
  analyzer_f analyzer;

  void* HANDLE; 	// plugin handle

public:
  //---------------------------------------------------------------------------
  error_e read( const char* filename ) {
    if( open( filename )  )
      // attempt to read the plugin file
      HANDLE = dlopen( filename, RTLD_LAZY );
    printf( "plugin: %s\n", filename );

    if( !HANDLE ) {
      std::cerr << " failed to read plugin from " << filename << std::endl;
      std::cerr << "  " << dlerror( ) << std::endl;
      return ERROR_OPEN;
    }

    // locate the analyzer function
    analyzer = (analyzer_f) dlsym(HANDLE, "analyze");
    const char* dlsym_error = dlerror( );
    if( dlsym_error ) {
      std::cerr << " warning: cannot load symbol 'analyze' from " << filename << std::endl;
      std::cerr << "        error follows: " << std::endl << dlsym_error << std::endl;
      return ERROR_LINK;
    }

    return ERROR_NONE;
  }
  //---------------------------------------------------------------------------
  void close( void ) {
    if( HANDLE != NULL )
      dlclose( HANDLE );
  }
  //---------------------------------------------------------------------------

private:
  //---------------------------------------------------------------------------
  error_e open( const char* filename ) {

    HANDLE = dlopen( filename, RTLD_LAZY );
    if( !HANDLE ) {
      //if( VERBOSE ) std::cerr << " failed to open plugin: " << filename << std::endl;
      //if( VERBOSE ) std::cerr << "  " << dlerror( ) << std::endl;
      return ERROR_OPEN;
    }
    return ERROR_NONE;
  }

};

//-----------------------------------------------------------------------------

} // namespace Analytics

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_PLUGIN_H_
