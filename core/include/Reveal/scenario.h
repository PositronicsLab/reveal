/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_SCENARIO_H_
#define _REVEAL_CORE_SCENARIO_H_

//-----------------------------------------------------------------------------

#include <stdio.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

class scenario_c {
public:
  scenario_c( void ) {}
  virtual ~scenario_c( void ) {}
 
  std::string name;
  unsigned trials;
  std::vector<std::string> uris;

  void print( void ) const {
    printf( "name[%s]", name.c_str() );
    printf( ", trials[%u]", trials );
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
