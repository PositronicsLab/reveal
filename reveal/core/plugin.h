/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_PLUGIN_H_
#define _REVEAL_CORE_PLUGIN_H_

//-----------------------------------------------------------------------------

#include "reveal/core/pointers.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class plugin_c {
private:
  void* HANDLE;

public:
  //---------------------------------------------------------------------------
  /// Default constructor
  plugin_c( void );
  //---------------------------------------------------------------------------
  /// Destructor
  virtual ~plugin_c( void );

  //---------------------------------------------------------------------------
  /// Loads a plugin from a path.  Fulfills the abstract module interface
  virtual error_e load( std::string path );

  //---------------------------------------------------------------------------
  // the set of plugin functions 
 
private:
  //---------------------------------------------------------------------------
  error_e read( std::string function_name, void* signiture_f, void* function );

  //---------------------------------------------------------------------------
  /// connects the plugin to the system and validates the plugin interface
  /// @param path the path to the plugin to read
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  error_e read( const char* path );

  //---------------------------------------------------------------------------
  /// opens the plugin as a dynamic library
  /// @param path the path to the plugin to read
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  error_e open( const char* path );

  //---------------------------------------------------------------------------
  /// closes an open plugin
  void close( void );

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_PLUGIN_H_
