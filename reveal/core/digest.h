/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

digest.h defines the digest_c data-structure that composes sets of scenario 
data 
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_DIGEST_
#define _REVEAL_CORE_DIGEST_

//----------------------------------------------------------------------------

#include "reveal/core/pointers.h"
#include <vector>
#include <assert.h>

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Core {
//----------------------------------------------------------------------------

class digest_c {
public:
  /// Default constructor
  digest_c( void ) { }
  /// Destructor
  virtual ~digest_c( void ) { }

  /// Gets a scenario from the digest given a valid index
  /// @param i the index of the scenario to return.  Must be less than scenarios
  /// @return the scenario that was requested
  scenario_ptr get_scenario( unsigned i ) {  
    assert( i < _scenarios.size() );
    return _scenarios[i];
  }

  /// Returns the number of scenarios in the digest
  /// @return the number of scenarios in the digest
  unsigned scenarios( void ) {
    return _scenarios.size();
  }

  /// Pushes a new scenario into the set of scenarios
  /// @param scenario the scenario to push into the digest
  void add_scenario( scenario_ptr scenario ) {
    _scenarios.push_back( scenario );
  }

/*
  /// Prints the contents of the digest to the console
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
  std::vector<scenario_ptr> _scenarios;  //< the set of scenarios in the digest
};

//----------------------------------------------------------------------------
} // namespace Core
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_DIGEST_
