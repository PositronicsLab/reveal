/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

digest.h defines the digest_c data-structure that composes sets of scenario 
data 
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_DIGEST_
#define _REVEAL_CORE_DIGEST_

//----------------------------------------------------------------------------

#include "Reveal/core/pointers.h"
#include <vector>
#include <assert.h>

//----------------------------------------------------------------------------

namespace Reveal {

//----------------------------------------------------------------------------

namespace Core {

//----------------------------------------------------------------------------

class digest_c {
public:
  digest_c( void ) { }
  virtual ~digest_c( void ) { }

  scenario_ptr get_scenario( unsigned i ) {  
    assert( i < _scenarios.size() );
    return _scenarios[i];
  }

  unsigned scenarios( void ) {
    return _scenarios.size();
  }

  void add_scenario( scenario_ptr scenario ) {
    _scenarios.push_back( scenario );
  }
/*
  void print( void ) {
    printf( "scenarios[%u]\n", _scenarios.size() );
    for( unsigned i = 0; i < _scenarios.size(); i++ ) {
      printf( "  id[%s]", _scenarios[i]->id.c_str() );
      printf( ", description[%s]", _scenarios[i]->description.c_str() );
      printf( ", trials[%u]\n", _scenarios[i]->trials );
    }
  }
*/
private:
  std::vector<scenario_ptr> _scenarios;
};

//----------------------------------------------------------------------------

} // namespace Core

//----------------------------------------------------------------------------

} // namespace Reveal

//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_DIGEST_
