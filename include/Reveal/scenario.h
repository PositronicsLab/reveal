/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SCENARIO_H_
#define _REVEAL_SCENARIO_H_

//-----------------------------------------------------------------------------

#include "net.pb.h"
#include <stdio.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class Scenario {
public:
  Scenario( void ) {}
  virtual ~Scenario( void ) {}
 
  std::string name;
  unsigned trials;
  std::vector<std::string> urls;

  void Print( void ) const {
    printf( "name[%s]", name.c_str() );
    printf( ", trials[%u]", trials );
    printf( ", urls[" );
    for( unsigned i = 0; i < urls.size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%s", urls.at(i).c_str() );
    }
    printf( "]\n" );
  }
};

//-----------------------------------------------------------------------------
/*
class ScenarioMessage {
public:
  ScenarioMessage( void );
  virtual ~ScenarioMessage( void );

  ScenarioMessage( Messages::Net::ScenarioRequest request );
  ScenarioMessage( Messages::Net::ScenarioResponse response );
};
*/
//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SCENARIO_H_
