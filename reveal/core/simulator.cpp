#include "reveal/core/simulator.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

simulator_c::simulator_c( void ) {
  _handle = NULL;
}

//-----------------------------------------------------------------------------
simulator_c::~simulator_c( void ) {
  if( _handle ) unload();
}

//-----------------------------------------------------------------------------
bool simulator_c::load( std::string path ) {
  if( _handle && _path == path ) return true; // already loaded

  _path = path;

  _handle = dlopen( _path.c_str(), RTLD_LAZY );
  if( !_handle ) return false;

  /// if the handle is null, open failed so report and return error
  if( !_handle ) {
    std::cerr << " failed to read plugin from " << _path << std::endl;
    std::cerr << "  " << dlerror( ) << std::endl;
    return false;
  }

  bool failed = false;
  // map interfaces.  If any fail, the handle is closed and failure returned
  if( !map_symbol( "ui_select_configuration" ) )  failed = true;
  if( !map_symbol( "ui_select_tuning" ) )         failed = true;
  if( !map_symbol( "build_package" ) )            failed = true;
  if( !map_symbol( "execute" ) )                  failed = true;
  if( !map_symbol( "set_request_trial" ) )        failed = true;
  if( !map_symbol( "set_submit_solution" ) )      failed = true;
  if( !map_symbol( "set_ipc_context" ) )          failed = true;

  if( failed ) {
    unload();
    return false;
  }

  // otherwise the plugin is loaded so return success
  return true;
}

//-----------------------------------------------------------------------------
void simulator_c::unload( void ) {
  if( _handle ) dlclose( _handle );
  _handle = NULL;
}

//-----------------------------------------------------------------------------
bool simulator_c::map_symbol( std::string symbol ) {

  assert( _handle );

  // verify that the requested symbol is valid 
  if( symbol == "ui_select_configuration" ) {
    //ui_select_configuration = (ui_select_configuration_f) dlsym( _handle, symbol.c_str() );
    ui_select_configuration = (ui_select_configuration_f) dlsym( _handle, "ui_select_configuration" );
  } else if( symbol == "ui_select_tuning" ) {
    ui_select_tuning = (ui_select_tuning_f) dlsym( _handle, symbol.c_str() );
  } else if( symbol == "build_package" ) {
    build_package = (build_package_f) dlsym( _handle, symbol.c_str() );
  } else if( symbol == "execute" ) {
    execute = (execute_f) dlsym( _handle, symbol.c_str() );
  } else if( symbol == "set_request_trial" ) {
    set_request_trial = (set_request_trial_f) dlsym( _handle, symbol.c_str() );
  } else if( symbol == "set_submit_solution" ) {
    set_submit_solution = (set_submit_solution_f) dlsym( _handle, symbol.c_str() );
  } else if( symbol == "set_ipc_context" ) {
    set_ipc_context = (set_ipc_context_f) dlsym( _handle, symbol.c_str() );
  } else {
    return false;
  }

  // query dl interface for any error
  const char* dlsym_error = dlerror( );

  // if the interface does not match the signature or cannot be located,
  // report and return failure
  if( dlsym_error ) {
    std::cerr << " warning: cannot load symbol '" << symbol << "' from " << _path << std::endl;
    std::cerr << "        error follows: " << std::endl << dlsym_error << std::endl;
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
