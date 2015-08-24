/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

transport_exchange.h defines the transport_exchange_c builder that encapsulates
and manages the exchange of Reveal data across the transport protocol.  The
exchange parses serialized protocols into class instances and builds serialized
protocols from class instances
-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SAMPLES_EXCHANGE_H_
#define _REVEAL_SAMPLES_EXCHANGE_H_

//----------------------------------------------------------------------------

#include <reveal/messages/samples.pb.h>
#include <reveal/messages/auth.pb.h>
#include "reveal/core/exchange.h"
#include "reveal/core/pointers.h"

#include <string>

//----------------------------------------------------------------------------
namespace Reveal {
//----------------------------------------------------------------------------
namespace Samples {
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

  virtual std::string type( void ) { return "benchmarks"; }

  virtual bool build( std::string& message );

  virtual bool parse( const std::string& message );

  /// The enumerated set of message origins
  enum origin_e {
    ORIGIN_UNDEFINED = 0,  //< an invalid message
    ORIGIN_SERVER,         //< the message originates from the server
    ORIGIN_CLIENT          //< the message originates from a client
  };

  /// The enumerated set of message errors
  enum error_e {
    ERROR_NONE = 0,                 //< no error detected with message
    ERROR_PARSE,                    //< unable to parse message, malformed
    ERROR_BUILD,                    //< unable to build message, incoherent
    ERROR_AUTHORIZATION,            //< authorization was invalid
    ERROR_BAD_SCENARIO_REQUEST,     //< scenario was invalid
    ERROR_BAD_TRIAL_REQUEST,        //< trial was invalid
    ERROR_BAD_SOLUTION_SUBMISSION   //< solution was malformed
  };

  /// The enumerated set of message types
  enum type_e {
    TYPE_UNDEFINED = 0,    //< an invalid message
    TYPE_ERROR,            //< the message is an error notification
    TYPE_HANDSHAKE,        //< the message is an attempt to handshake
    TYPE_DIGEST,           //< the message is a digest
    TYPE_EXPERIMENT,       //< the message is an experiment
    TYPE_TRIAL,            //< the message is a trial
    TYPE_SOLUTION,         //< the message is a solution
    TYPE_STEP,             //< the message is a step command
    TYPE_EXIT              //< the message is an exit command
  };
/*
  enum command_e {
    COMMAND_UNDEFINED = 0,
    COMMAND_STEP,
    COMMAND_EXIT
  }
*/
  /// Sets the origin of the message
  /// @param origin the origin of the message
  void set_origin( origin_e origin );
  /// Gets the origin of the message
  /// @return the origin of the message
  origin_e get_origin( void );

  /// Sets the authorization of the message
  /// @param authorization the authorization of the message
  void set_authorization( Reveal::Core::authorization_ptr authorization );
  /// Gets the authorization of the message
  /// @return the authorization of the message
  Reveal::Core::authorization_ptr get_authorization( void );

  /// Sets the error of the message
  /// @param error the error of the message
  void set_error( error_e error );
  /// Gets the error of the message
  /// @return the error of the message
  error_e get_error( void );

  /// Sets the type of the message
  /// @param type the type of the message
  void set_type( type_e type );
  /// Gets the type of the message
  /// @return the type of the message
  type_e get_type( void );

  /// Sets the digest of the message
  /// @param digest the digest of the message
  void set_digest( Reveal::Core::digest_ptr digest );
  /// Gets the digest of the message
  /// @return the digest of the message
  Reveal::Core::digest_ptr get_digest( void );

  /// Sets the experiment of the message
  /// @param experiment the experiment of the message
  void set_experiment( Reveal::Core::experiment_ptr experiment );
  /// Gets the experiment of the message
  /// @return the experiment of the message
  Reveal::Core::experiment_ptr get_experiment( void );

  /// Sets the scenario of the message
  /// @param scenario the scenario of the message
  void set_scenario( Reveal::Core::scenario_ptr scenario );
  /// Gets the scenario of the message
  /// @return the scenario of the message
  Reveal::Core::scenario_ptr get_scenario( void );

  /// Sets the trial of the message
  /// @param trial the trial of the message
  void set_trial( Reveal::Core::trial_ptr trial );
  /// Gets the trial of the message
  /// @return the trial of the message
  Reveal::Core::trial_ptr get_trial( void );

  /// Sets the solution of the message
  /// @param solution the solution of the message
  void set_solution( Reveal::Core::solution_ptr solution );
  /// Gets the solution of the message
  /// @return the solution of the message
  Reveal::Core::solution_ptr get_solution( void );

  /// Builds a server experiment message from the provided parameters
  /// @param message the serialized message returned on success
  /// @param auth the authorization to assign to the message
  /// @param scenario the scenario to assign to the message
  /// @param experiment the experiment to assign to the message
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_server_experiment( std::string& message, Reveal::Core::authorization_ptr auth, Reveal::Core::scenario_ptr scenario, Reveal::Core::experiment_ptr experiment );

  /// Builds a server trial message from the provided parameters
  /// @param message the serialized message returned on success
  /// @param auth the authorization to assign to the message
  /// @param experiment the experiment to assign to the message
  /// @param trial the trial to assign to the message
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_server_trial( std::string& message, Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::trial_ptr trial );

  /// Builds a server solution message from the provided parameters
  /// @param message the serialized message returned on success
  /// @param auth the authorization to assign to the message
  /// @param experiment the experiment to assign to the message
  /// @param solution the solution to assign to the message
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_client_solution( std::string& message, Reveal::Core::authorization_ptr auth, Reveal::Core::experiment_ptr experiment, Reveal::Core::solution_ptr solution );

  /// Builds a server step command message from the provided parameters
  /// @param message the serialized message returned on success
  /// @param auth the authorization to assign to the message
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_server_command_step( std::string& message, Reveal::Core::authorization_ptr auth );

  /// Builds a server exit command message from the provided parameters
  /// @param message the serialized message returned on success
  /// @param auth the authorization to assign to the message
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_server_command_exit( std::string& message, Reveal::Core::authorization_ptr auth );

  /// Parses a server experiment message and returns a set of class instances
  /// containing the message data
  /// @param message the serialized message to be parsed
  /// @param auth the authorization returned from parsing the message
  /// @param scenario the scenario returned from parsing the message
  /// @param experiment the experiment returned from parsing the message
  /// @return returns ERROR_NONE on successfully parsing the message OR another
  ///         enumerated error value indicating the error that occurred 
  bool parse_server_experiment( const std::string& message, Reveal::Core::authorization_ptr& auth, Reveal::Core::scenario_ptr& scenario, Reveal::Core::experiment_ptr& experiment );

  /// Parses a client solution message and returns a set of class instances
  /// containing the message data
  /// @param message the serialized message to be parsed
  /// @param auth the authorization returned from parsing the message
  /// @param experiment the experiment returned from parsing the message
  /// @param solution the solution returned from parsing the message
  /// @return returns ERROR_NONE on successfully parsing the message OR another
  ///         enumerated error value indicating the error that occurred 
  bool parse_client_solution( const std::string& message, Reveal::Core::authorization_ptr& auth, Reveal::Core::experiment_ptr& experiment, Reveal::Core::solution_ptr& solution );

private:
  origin_e            _origin;         //< the message's origin
  type_e              _type;           //< the message's type
  error_e             _error;          //< the message's error

  Reveal::Core::authorization_ptr _authorization;//< the message's authorization
  Reveal::Core::digest_ptr        _digest;       //< the message's digest
  Reveal::Core::experiment_ptr    _experiment;   //< the message's experiment
  Reveal::Core::scenario_ptr      _scenario;     //< the message's scenario
  Reveal::Core::trial_ptr         _trial;        //< the message's trial
  Reveal::Core::solution_ptr      _solution;     //< the message's solution

  /// Builds the authorization portion of a message
  /// @param message the message to build into
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_authorization( Reveal::Messages::Samples::Message* message );

  /// Builds a client message
  /// @param message the message to build into
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_client_message( Reveal::Messages::Samples::Message* message );

  /// Builds a server message
  /// @param message the message to build into
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool build_server_message( Reveal::Messages::Samples::Message* message );

  /// Maps the origin of a message to the instance variables
  /// @param message the message to map
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool map_origin( Reveal::Messages::Samples::Message* message );

  /// Maps the type of a message to the instance variables
  /// @param message the message to map
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool map_type( Reveal::Messages::Samples::Message* message );

  /// Maps the authorization of a message to the instance variables
  /// @param message the message to map
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool map_authorization( Reveal::Messages::Samples::Message* message );

  /// Maps a client message to appropriate instance variables
  /// @param message the message to map
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool map_client_message( Reveal::Messages::Samples::Message* message );

  /// Maps a server message to appropriate instance variables
  /// @param message the message to map
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool map_server_message( Reveal::Messages::Samples::Message* message );

  /// Writes the class digest reference into a message
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool write_digest( Reveal::Messages::Samples::Message* message );

  /// Reads the message digest into the class digest reference
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool read_digest( Reveal::Messages::Samples::Message* message );

  /// Writes the class scenario reference into a message
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool write_scenario( Reveal::Messages::Samples::Message* message );

  /// Reads the message scenario into the class digest reference
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool read_scenario( Reveal::Messages::Samples::Message* message );

  /// Writes the class experiment reference into a message
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool write_experiment( Reveal::Messages::Samples::Message* message );

  /// Reads the message experiment into the class digest reference
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool read_experiment( Reveal::Messages::Samples::Message* message );

  /// Writes the class trial reference into a message
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool write_trial( Reveal::Messages::Samples::Message* message );

  /// Reads the message trial into the class digest reference
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool read_trial( Reveal::Messages::Samples::Message* message );

  /// Writes the class solution reference into a message
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool write_solution( Reveal::Messages::Samples::Message* message );

  /// Reads the message solution into the class digest reference
  /// @param message the message to write to
  /// @return returns ERROR_NONE on successfully building the message OR another
  ///         enumerated error value indicating the error that occurred
  bool read_solution( Reveal::Messages::Samples::Message* message );
};

//----------------------------------------------------------------------------
} // namespace Samples
//----------------------------------------------------------------------------
} // namespace Reveal
//----------------------------------------------------------------------------

#endif //  _REVEAL_SAMPLES_EXCHANGE_H_
