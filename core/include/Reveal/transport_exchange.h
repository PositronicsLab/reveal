/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

transport_exchange.h defines the transport_exchange_c builder that encapsulates
and manages the exchange of Reveal data across the transport protocol.  The
exchange parses serialized protocols into class instances and builds serialized
protocols from class instances
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_TRANSPORT_EXCHANGE_
#define _REVEAL_CORE_TRANSPORT_EXCHANGE_

//----------------------------------------------------------------------------

#include <Reveal/data.pb.h>
#include <Reveal/net.pb.h>

#include <Reveal/pointers.h>

#include <string>

//----------------------------------------------------------------------------

namespace Reveal {

//----------------------------------------------------------------------------

namespace Core {

//----------------------------------------------------------------------------

class transport_exchange_c {
public:

  enum origin_e {
    ORIGIN_UNDEFINED = 0,
    ORIGIN_SERVER,
    ORIGIN_CLIENT
  };

  enum error_e {
    ERROR_NONE = 0,
    ERROR_PARSE,
    ERROR_BUILD,
    ERROR_AUTHORIZATION,
    ERROR_BAD_SCENARIO_REQUEST,
    ERROR_BAD_TRIAL_REQUEST,
    ERROR_BAD_SOLUTION_SUBMISSION
  };

  enum type_e {
    TYPE_UNDEFINED = 0,
    TYPE_ERROR,
    TYPE_HANDSHAKE,
    TYPE_DIGEST,
    TYPE_EXPERIMENT,
    TYPE_TRIAL,
    TYPE_SOLUTION
  };

  transport_exchange_c( void );
  virtual ~transport_exchange_c( void );

  static void open( void );
  static void close( void ); 
  
  void reset( void );

  void set_origin( origin_e origin );
  origin_e get_origin( void );

  void set_authorization( authorization_ptr authorization );
  authorization_ptr get_authorization( void );

  void set_error( error_e error );
  error_e get_error( void );

  void set_type( type_e type );
  type_e get_type( void );

  void set_digest( digest_ptr digest );
  digest_ptr get_digest( void );

  void set_experiment( experiment_ptr experiment );
  experiment_ptr get_experiment( void );

  void set_scenario( scenario_ptr scenario );
  scenario_ptr get_scenario( void );

  void set_trial( trial_ptr trial );
  trial_ptr get_trial( void );

  void set_solution( solution_ptr solution );
  solution_ptr get_solution( void );

  error_e build( std::string& message );
  error_e parse( const std::string& message );

private:
  origin_e            _origin;
  type_e              _type;
  error_e             _error;

  authorization_ptr   _authorization;
  digest_ptr          _digest;
  experiment_ptr      _experiment;
  scenario_ptr        _scenario;
  trial_ptr           _trial;
  solution_ptr        _solution;


  error_e build_authorization( Reveal::Core::Messages::Net::Message* message );
  error_e build_client_message( Reveal::Core::Messages::Net::Message* message );
  error_e build_server_message( Reveal::Core::Messages::Net::Message* message );

  error_e map_origin( Reveal::Core::Messages::Net::Message* message );
  error_e map_type( Reveal::Core::Messages::Net::Message* message );
  error_e map_authorization( Reveal::Core::Messages::Net::Message* message );
  error_e map_client_message( Reveal::Core::Messages::Net::Message* message );
  error_e map_server_message( Reveal::Core::Messages::Net::Message* message );
};

//----------------------------------------------------------------------------

} // namespace Core

//----------------------------------------------------------------------------

} // namespace Reveal

//----------------------------------------------------------------------------

#endif // _REVEAL_CORE_TRANSPORT_EXCHANGE_
