/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_EXCHANGE_H_
#define _REVEAL_CORE_EXCHANGE_H_

//----------------------------------------------------------------------------

#include "reveal/core/pointers.h"

#include <string>

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Core {
//----------------------------------------------------------------------------

class exchange_c;
typedef boost::shared_ptr<Reveal::Core::exchange_c> exchange_ptr;

//----------------------------------------------------------------------------
class exchange_c {
public:

  virtual bool open( void ) = 0;
  virtual void close( void ) = 0; 
  virtual void reset( void ) = 0;

  virtual std::string type( void ) = 0;

  virtual bool build( std::string& message ) = 0;
  virtual bool parse( const std::string& message ) = 0;
};

//----------------------------------------------------------------------------
} // namespace Core
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_EXCHANGE_H_
