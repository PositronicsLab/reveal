/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SCENARIO_H_
#define _REVEAL_SCENARIO_H_

//-----------------------------------------------------------------------------

#include <stdio.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class scenario_c {
public:
  scenario_c( void ) {}
  virtual ~scenario_c( void ) {}
 
  std::string name;
  unsigned trials;
  std::vector<std::string> urls;

  void print( void ) const {
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

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SCENARIO_H_
