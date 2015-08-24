#include "reveal/analytics/script.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Analytics {
//-----------------------------------------------------------------------------

script_c::script_c( void ) { 

}

script_c::~script_c( void ) {

}

Reveal::Analytics::error_e script_c::load( std::string file ) {
  assert( file != "" );
  return ERROR_NONE;
}

Reveal::Analytics::error_e script_c::analyze( Reveal::Core::solution_set_ptr in, Reveal::Core::analysis_ptr& out ) {
  assert( in );
  out = Reveal::Core::analysis_ptr( new Reveal::Core::analysis_c() );
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------
} // namespace Analytics
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
