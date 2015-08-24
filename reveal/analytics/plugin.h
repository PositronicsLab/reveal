/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

plugin.h defines the plugin_c implementation of the module_c interface.  Plugins
use the DL library to dynamically link analyzers implemented in c++ and compiled
into shared objects 
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_PLUGIN_H_
#define _REVEAL_ANALYTICS_PLUGIN_H_

//-----------------------------------------------------------------------------

#include "reveal/core/pointers.h"
#include "reveal/core/solution_set.h"
#include "reveal/core/analysis.h"

#include "reveal/analytics/types.h"
#include "reveal/analytics/module.h"

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
  plugin_c( void );
  //---------------------------------------------------------------------------
  /// Destructor
  virtual ~plugin_c( void );

  //---------------------------------------------------------------------------
  /// Loads a plugin from a path.  Fulfills the abstract module interface
  /// @param path the path to the plugin
  /// @return if load in successful returns ERROR_NONE otherwise returns an 
  ///         enumerated value describing the error
  virtual error_e load( std::string path );

  //---------------------------------------------------------------------------
  /// Executes the analyze function of the loaded plugin.  Fulfills the 
  /// abstract module interface
  /// @param in the solution set submitted for analysis
  /// @param out the result of the module's analytics computation
  /// @return on success returns ERROR_NONE otherwise returns an enumerated
  ///         value that gives some information on the error 
  virtual error_e analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out );
  
private:
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
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_PLUGIN_H_
