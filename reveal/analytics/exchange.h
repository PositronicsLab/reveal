/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

transport_exchange.h defines the transport_exchange_c builder that encapsulates
and manages the exchange of Reveal data across the transport protocol.  The
exchange parses serialized protocols into class instances and builds serialized
protocols from class instances
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_EXCHANGE_H_
#define _REVEAL_ANALYTICS_EXCHANGE_H_

//----------------------------------------------------------------------------

#include "reveal/core/exchange.h"
#include "reveal/core/pointers.h"

#include <reveal/messages/auth.pb.h>
#include <reveal/messages/analytics.pb.h>

#include <string>

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Analytics {
//----------------------------------------------------------------------------

class exchange_c : public Reveal::Core::exchange_c {
public:

  /// Default constructor
  exchange_c( void );

  /// Destructor
  virtual ~exchange_c( void );

  /// Opens the exchange.  Should be done once by the parent process before any
  /// intercommunication is attempted
  virtual bool open( void );

  /// Closes the exchange.  Should be done once by the parent process when all
  /// other processes have stopped intercommunicating
  virtual void close( void ); 

  /// Clears any information buffered in an instantiated exchange  
  virtual void reset( void );

  virtual std::string type( void ) { return "analytics"; }

  virtual bool build( std::string& message );
  virtual bool parse( const std::string& message );

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /// The enumerated set of message origins
  enum origin_e {
    ORIGIN_UNDEFINED = 0,  //< an invalid message
    ORIGIN_ADMIN,         
    ORIGIN_WORKER          
  };
  enum error_e {
    ERROR_NONE = 0,
    ERROR_GENERAL,
    ERROR_AUTHORIZATION
  };
  /// The enumerated set of message types
  enum type_e {
    TYPE_UNDEFINED = 0,    //< an invalid message
    TYPE_ERROR,            //< the message is an error notification
    TYPE_HANDSHAKE,        //< the message is an attempt to handshake
    TYPE_REQUEST, 
    TYPE_RESPONSE,
    TYPE_COMMAND 
  };
  enum request_e {
    REQUEST_JOB = 0,
    REQUEST_RECEIPT
  };

  enum response_e {
    RESPONSE_JOB_ISSUED = 0,
    RESPONSE_JOB_DENIED,
    RESPONSE_RECEIPT_AUTHORIZED,
    RESPONSE_RECEIPT_REJECTED
  };
  enum command_e {
    COMMAND_EXIT = 0
  };
  /// Sets the origin of the message
  /// @param origin the origin of the message
  void set_origin( origin_e origin );
  /// Gets the origin of the message
  /// @return the origin of the message
  origin_e get_origin( void );

  /// Sets the error of the message
  /// @param error the error of the message
  void set_error( error_e error );
  /// Gets the error of the message
  /// @return the error of the message
  error_e get_error( void );

  /// Sets the authorization of the message
  /// @param authorization the authorization of the message
  void set_authorization( Reveal::Core::authorization_ptr authorization );
  /// Gets the authorization of the message
  /// @return the authorization of the message
  Reveal::Core::authorization_ptr get_authorization( void );

  /// Sets the type of the message
  /// @param type the type of the message
  void set_type( type_e type );
  /// Gets the type of the message
  /// @return the type of the message
  type_e get_type( void );

  /// Sets the request of the message
  /// @param request the request of the message
  void set_request( request_e request );
  /// Gets the request of the message
  /// @return the request of the message
  request_e get_request( void );

  /// Sets the response of the message
  /// @param response the response of the message
  void set_response( response_e response );
  /// Gets the response of the message
  /// @return the response of the message
  response_e get_response( void );

  /// Sets the command of the message
  /// @param command the command of the message
  void set_command( command_e command );
  /// Gets the command of the message
  /// @return the command of the message
  command_e get_command( void );

private:
  origin_e            _origin;         //< the message's origin
  type_e              _type;           //< the message's type
  error_e             _error;          //< the message's error
  request_e           _request;
  response_e          _response;
  command_e           _command;

  Reveal::Core::authorization_ptr _authorization;//< the message's authorization

  bool map_origin( Reveal::Messages::Analytics::Message* message );
  bool map_type( Reveal::Messages::Analytics::Message* message );
  bool map_request( Reveal::Messages::Analytics::Message* message );
  bool map_response( Reveal::Messages::Analytics::Message* message );
  bool map_command( Reveal::Messages::Analytics::Message* message );
  bool map_authorization( Reveal::Messages::Analytics::Message* message );

  bool map_admin_message( Reveal::Messages::Analytics::Message* message );
  bool map_worker_message( Reveal::Messages::Analytics::Message* message );

  bool build_authorization( Reveal::Messages::Analytics::Message* message );
  bool build_admin_message( Reveal::Messages::Analytics::Message* message );
  bool build_worker_message( Reveal::Messages::Analytics::Message* message );

};

//----------------------------------------------------------------------------
} // namespace Analytics
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------

#endif //  _REVEAL_ANALYTICS_EXCHANGE_H_
