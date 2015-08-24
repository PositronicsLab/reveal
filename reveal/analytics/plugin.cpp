#include "reveal/analytics/plugin.h"

#include <dlfcn.h>              //POSIX

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

plugin_c::plugin_c( void ) { 
  HANDLE = NULL; 
}

//-----------------------------------------------------------------------------
plugin_c::~plugin_c( void ) { 
  if( HANDLE ) close(); 
}

//-----------------------------------------------------------------------------
error_e plugin_c::load( std::string path ) {
  return read( path.c_str() );
}

//-----------------------------------------------------------------------------
error_e plugin_c::analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) {
  assert( HANDLE );
  return analyzer( in, out );
}
  
//-----------------------------------------------------------------------------
error_e plugin_c::read( const char* path ) {

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

  // if the analyzer function does not match the signature or cannot be 
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

//-----------------------------------------------------------------------------
error_e plugin_c::open( const char* path ) {
  HANDLE = dlopen( path, RTLD_LAZY );
  if( !HANDLE ) return ERROR_OPEN;
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
/// closes an open plugin
void plugin_c::close( void ) {
  if( HANDLE ) dlclose( HANDLE );
  HANDLE = NULL;
}

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

