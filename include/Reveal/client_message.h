/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_CLIENT_MESSAGE_H_
#define _REVEAL_CLIENT_MESSAGE_H_

//-----------------------------------------------------------------------------

#include <Reveal/pointers.h>
#include "net.pb.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class ClientMessage {
public:
  ClientMessage( void );
//  ClientMessage( Messages::Net::ClientRequest msg );
  virtual ~ClientMessage( void );

  enum Type {
    UNDEFINED = 0,
    ERROR,
    SCENARIO,
    TRIAL,
    SOLUTION
  };

  enum Error {
    ERR_NONE = 0,
    ERR_RESPONSE
  };

  bool Parse( const std::string& serial );
  std::string Serialize( void );
  
  Type getType( void );

  ScenarioPtr getScenario( void );
  void setScenario( ScenarioPtr scenario );

  TrialPtr getTrial( void );
  void setTrial( TrialPtr trial );

  SolutionPtr getSolution( void );
  void setSolution( SolutionPtr solution );

  Error getError( void );
  void setError( const Error& error );

private:
  Type _type;
  Error _error;

  Messages::Net::ClientRequest _request;
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_MESSAGE_H_
