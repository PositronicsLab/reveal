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

class client_message_c {
public:
  client_message_c( void );
  virtual ~client_message_c( void );

  enum type_e {
    UNDEFINED = 0,
    ERROR,
    SCENARIO,
    TRIAL,
    SOLUTION
  };

  enum error_e {
    ERR_NONE = 0,
    ERR_RESPONSE
  };

  bool parse( const std::string& serial );
  std::string serialize( void );
  
  type_e get_type( void );

  scenario_ptr get_scenario( void );
  void set_scenario( scenario_ptr scenario );

  trial_ptr get_trial( void );
  void set_trial( trial_ptr trial );

  solution_ptr get_solution( void );
  void set_solution( solution_ptr solution );

  error_e get_error( void );
  void set_error( const error_e& error );

private:
  type_e _type;
  error_e _error;

  Messages::Net::ClientRequest _request;
};

//-----------------------------------------------------------------------------

}  // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_MESSAGE_H_
