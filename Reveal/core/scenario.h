/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

scenario.h defines the scenario_c data-structure that contains scenario data.
A scenario is the priniciple set of data required to interact between client
and server
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SCENARIO_H_
#define _REVEAL_CORE_SCENARIO_H_

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "Reveal/core/pointers.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/xml.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class scenario_c {
public:
  scenario_c( void ) {}
  virtual ~scenario_c( void ) {}

  std::string id;
  std::string package_id;
  std::string description;
  std::vector<std::string> uris;

  double sample_rate;
  double sample_start_time;
  double sample_end_time;

  void print( void ) const {
    printf( "id[%s]", id.c_str() );
    printf( ", sample_rate[%f]", sample_rate );
    printf( ", sample_start_time[%f]", sample_start_time );
    printf( ", sample_end_time[%f]", sample_end_time );
    printf( ", uris[" );
    for( unsigned i = 0; i < uris.size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%s", uris.at(i).c_str() );
    }
    printf( "]\n" );
  }
};

//-----------------------------------------------------------------------------
}  // namespace Core
//-----------------------------------------------------------------------------
}  // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SCENARIO_H_
