/*-----------------------------------------------------------------------------
James R Taylor:jrt@gwu.edu

-----------------------------------------------------------------------------*/

#ifndef _REVEAL_SERVER_MESSAGE_H_
#define _REVEAL_SERVER_MESSAGE_H_

//-----------------------------------------------------------------------------

#include <Reveal/pointers.h>
#include "net.pb.h"

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

class ServerMessage {
public:
  ServerMessage( void );
  //ServerMessage( Messages::Net::ServerResponse msg );
  virtual ~ServerMessage( void );

  enum Type {
    UNDEFINED = 0,
    ERROR,
    SCENARIO,
    TRIAL,
    SOLUTION
  };

  enum Error {
    ERR_NONE = 0,
    ERR_REQUEST
  };

  bool Parse( const std::string& serial );
  std::string Serialize( void );

  Type getType( void );
  //void setType( const Type& type );

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

  Messages::Net::ServerResponse _response;
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_SERVER_MESSAGE_H_
