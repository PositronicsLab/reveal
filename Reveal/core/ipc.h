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
  /// The enumerated set of error codes that pipe operation may return
  enum error_e {
    ERROR_NONE = 0,     //< indicates an operation was successful
    ERROR_CONTEXT,      //< indicates that the context was invalid
    ERROR_SOCKET,       //< indicates that the socket was inaccessible
    ERROR_LIMIT,        //< indicates that a limit was exceeded
    ERROR_ADDRESS,      //< indicates that the socket address was invalid
    ERROR_EMPTY,        //< indicates that a message was empty
    ERROR_MODE,         //< indicates that a mode was invalid
    ERROR_STATE,        //< indicates that a state was invalid
    ERROR_OPTION,       //< indicates that an option was invalid
    ERROR_INTERRUPT     //< indicates that communication was interrupted
  };

  /// Default constructor for listener pipes
  /// @param port the port to listen on
  /// @param context the communication layer context.  All contexts within a
  ///        process are shared.  A NULL context instructs the pipe to create
  ///        this shared context
  pipe_c( const unsigned& port, void* context = NULL );

  /// Parameterized constructor for sender pipes
  /// @param host the host to connect to
  /// @param port the host port to connect to
  /// @param context the communication layer context.  All contexts within a
  ///        process are shared.  A NULL context instructs the pipe to create
  ///        this shared context
  pipe_c( const std::string& host, const unsigned& port, void* context = NULL );

  /// Parameterized constructor for internal system pipes
  /// @param id the name of the internal channel to communicate on
  /// @param bind true if this process or thread is to create the channel, 
  ///        server side responsibility, or false if the channel is created by
  ///        another process, client side expectation
  /// @param context the communication layer context.  All contexts within a
  ///        process are shared.  A NULL context instructs the pipe to create
  ///        this shared context
  pipe_c( const std::string& id, bool bind = false, void* context = NULL );

  /// Destructor
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

  /// The pipe's communication context
  /// @return a communication context
  void* context( void );

  /// The pipe's socket
  /// @return the pipe's socket
  void* socket( void );

private:

  bool _binder;          //< indicates this process bound the socket
  bool _created_context; //< indicates this process created the context
  bool _interthread;     //< indicates the pipe is only between system resources

  std::string _host;  //< the name or IP of the host this pipe connects to
  std::string _id;    //< the identifier assigned to the socket
  unsigned _port;     //< the port number of the external socket
  bool _open;         //< whether or not this connection is currently open

  void* _context;     //< the pipe's communication context
  void* _socket;      //< the pipe's socket

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
