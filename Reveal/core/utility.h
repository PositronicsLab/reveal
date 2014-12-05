/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

utility.h defines helper functions.  
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_UTILITY_H_
#define _REVEAL_CORE_UTILITY_H_

//-----------------------------------------------------------------------------

#include <string>
#include <vector>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------
void bin_to_text( std::string in, std::string& out, bool& plain_text ) {

  char buffer[2];
  plain_text = true;  

  for( int i = 0; i < in.size(); i++ )
    if( (unsigned char)in[i] < 32 || (unsigned char)in[i] > 127 )
      plain_text = false;

  if( plain_text ) {
    out = in;
  } else {
    for( int i = 0; i < in.size(); i++ ) {
      sprintf( buffer, "%02X", (unsigned char)in[i] );
      out.insert( out.size(), buffer, 2 );
    }
  }
}

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_UTILITY_H_
