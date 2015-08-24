/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

connection.h defines the connection_c adapter that encapsulates 0MQ (ZeroMQ) 
and manages the underlying connection using a general device interface model.  
The transport behavior adheres to the request-reply message paradigm.
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_CONNECTION_H_
#define _REVEAL_CORE_CONNECTION_H_

//-----------------------------------------------------------------------------

#include <string>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

/// The size of the send buffer.  Messages are sent in chunks.
#define SND_BUFFER_SZ   1024      // Small for validation. TODO:Increase
/// The size of the receive buffer.  Must be larger than the send buffer.
#define RCV_BUFFER_SZ   SND_BUFFER_SZ + 1 

//-----------------------------------------------------------------------------

class connection_c {
public:
  enum role_e {
    UNDEFINED = 0,
    ROUTER,       //< External server binding, accepts client connections.
    DEALER,       //< Internal server binding, ipc routing server-worker.
    WORKER,       //< Internal server connection, ipc routing endpoint, worker.
    CLIENT        //< Client connection, makes connections to servers.
//    IPC_SERVER,   //< Internal ipc connection server endpoint
//    IPC_CLIENT    //< Internal ipc connection client endpoint
  };

  enum error_e {
    ERROR_NONE = 0,   //< connection operation successful
    ERROR_CONTEXT,    //< the connection context was invalid
    ERROR_SOCKET,     //< the connection socket had an error
    ERROR_LIMIT,      //< the connection buffer overflowed its limit
    ERROR_ADDRESS,    //< the connection address was invalid
    ERROR_EMPTY,      //< the packet is empty
    ERROR_MODE,       //< the mode is invalid
    ERROR_STATE,      //< the socket state is invalid
    ERROR_OPTION,     //< a socket option is invalid
    ERROR_INTERRUPT   //< the communication was interrupted
  };

  /// Default constructor.
  connection_c( void );

  /// Server external binding constructor, role = ROUTER.
  /// @param port the external port for the server to bind to and accept 
  ///        connections on.
  connection_c( const unsigned& port );

  connection_c( const unsigned& port, void* context );

  /// Server internal connection/binding constructor, role = DEALER or WORKER.
  /// @param role the role {DEALER, WORKER} that this internal connection will
  ///        play.
  /// @param context the 0MQ context that is created by creating a ROUTER.  Must
  ///        create a ROUTER before creating any other internal connections.
  connection_c( const role_e& role, void* context );

  /// Client connection constructor, role = CLIENT.
  /// @param host the uri of the server that the client will connect to.
  /// @port the port that the server listens on.
  connection_c( const std::string& host, const unsigned& port );
/*
  /// Server internal ipc constructor, role = IPC.
  /// @param role the role {IPC} that this internal connection will play.
  /// @param context the 0MQ context that is created by creating a ROUTER.  Must
  ///        create a ROUTER before creating any other internal connections.
  /// @param id the identifier of the port to communicate on
  connection_c( const role_e& role, void* context, std::string id );
*/

  /// Default destructor.
  virtual ~connection_c( void );

  /// Opens the connection.
  /// @return returns ERROR_NONE if the connection was successfully opened;
  /// otherwise returns enumerated error.
  error_e open( void );

  /// Closes the connection.
  /// @return returns ERROR_NONE if the connection was successfully closed;
  /// otherwise returns enumerated error.
  error_e close( void );

  /// Blocking read from the connection.
  /// @param msg the string read from the connection on success.
  /// @return returns ERROR_NONE if the read was successful; otherwise returns 
  /// enumerated error.
  error_e read( std::string& msg );

  /// Non-blocking write to the connection. 
  /// @param msg the string to write to the connection.
  /// @return returns ERROR_NONE if the read was successful; otherwise returns 
  /// enumerated error.
  error_e write( const std::string& msg );

  /// Only valid for a ROUTER, routes connections to workers via a dealer.
  void route( connection_c& dealer );

  /// The 0MQ context created either by constructing a ROUTER or a CLIENT.
  void* context( void );

  /// The 0MQ socket
  void* socket( void );

private:
  /// The role of this connection.
  role_e _role;

  /// The host that the client is to connect to or is connected to.
  std::string _host;

  /// The identifier assigned to the socket
  std::string _id;

  /// The external connection port of the server.
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

#endif // _REVEAL_CORE_CONNECTION_H_
