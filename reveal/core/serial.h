/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

-----------------------------------------------------------------------------*/
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

  /// Defines the serialize interface for writing instance data into a stream
  /// @param stream the stream to serialize into
  /// @param delimiter the delimiter to insert between values
  /// @return the stream that was serialized into
  virtual std::stringstream& serialize( std::stringstream& stream, char delimiter ) = 0;

  /// Defines the deserialize interface for reading instance data from a stream
  /// @param stream the stream to read data from
  /// @return returns true if the instance was able to successfully deserialize
  ///         from the stream OR false if deserialization failed for any reason
  virtual bool deserialize( std::stringstream& stream ) = 0;

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_SERIAL_H_
