#ifndef _REVEAL_CORE_SERIAL_H_
#define _REVEAL_CORE_SERIAL_H_

#include <sstream>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class serial_c {
public:

  virtual std::stringstream& serialize( std::stringstream& stream, char delimiter ) = 0;
  virtual bool deserialize( std::stringstream& stream ) = 0;

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SERIAL_H_
