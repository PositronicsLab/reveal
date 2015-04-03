/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

ipc.h defines the pipe_c adapter that encapsulates 0MQ (ZeroMQ) 
and manages an underlying interprocess connection using a general device 
interface model.  The transport behavior adheres to a two-way read blocking,
write non-blocking paradigm.
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_IPC_H_
#define _REVEAL_CORE_IPC_H_

//-----------------------------------------------------------------------------

#include <zmq.h>
#include <string>
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Core {

//-----------------------------------------------------------------------------

/// The size of the send buffer.  Messages are sent in chunks.
#define PIPE_SND_BUFFER_SZ   64        // Small for validation. TODO:Increase
/// The size of the receive buffer.  Must be larger than the send buffer.
#define PIPE_RCV_BUFFER_SZ   PIPE_SND_BUFFER_SZ + 1 

//-----------------------------------------------------------------------------

class pipe_c;
typedef boost::shared_ptr<pipe_c> pipe_ptr;

//-----------------------------------------------------------------------------

class pipe_c {
public:

  enum error_e {
    ERROR_NONE = 0,
    ERROR_CONTEXT,
    ERROR_SOCKET,
    ERROR_LIMIT,
    ERROR_ADDRESS,
    ERROR_EMPTY,
    ERROR_MODE,
    ERROR_STATE,
    ERROR_OPTION,
    ERROR_INTERRUPT
  };

  /// Default constructor.
  pipe_c( const unsigned& port, void* context = NULL );

  pipe_c( const std::string& host, const unsigned& port, void* context = NULL );

  pipe_c( const std::string& id, bool bind = false, void* context = NULL );

  /// Default destructor.
  virtual ~pipe_c( void );

  /// Opens the pipe.
  /// @return returns ERROR_NONE if the connection was successfully opened;
  /// otherwise returns enumerated error.
  error_e open( void );

  /// Closes the pipe.
  /// @return returns ERROR_NONE if the connection was successfully closed;
  /// otherwise returns enumerated error.
  error_e close( void );

  /// Blocking read from the pipe.
  /// @param msg the string read from the connection on success.
  /// @return returns ERROR_NONE if the read was successful; otherwise returns 
  /// enumerated error.
  error_e read( std::string& msg );

  /// Non-blocking write to the pipe. 
  /// @param msg the string to write to the connection.
  /// @return returns ERROR_NONE if the read was successful; otherwise returns 
  /// enumerated error.
  error_e write( const std::string& msg );

  /// The 0MQ context
  void* context( void );

  /// The 0MQ socket
  void* socket( void );

private:

  bool _binder;
  bool _created_context;
  bool _interthread; 

  /// Any host the pipe is connected to.
  std::string _host;

  /// The identifier assigned to the socket
  std::string _id;

  /// Any external connection port.
  unsigned _port;

  /// Whether or not this connection is currently open.
  bool _open;

  /// The 0MQ context.
  void* _context;
  /// The 0MQ socket.
  void* _socket;

  /// Generates a connection string based on role.
  /// @return the connection string generated.
  std::string connection_string( void );
};

//-----------------------------------------------------------------------------

}  // namespace Core

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_IPC_H_
