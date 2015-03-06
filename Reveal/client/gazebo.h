#ifndef _REVEAL_CLIENT_GAZEBO_H_
#define _REVEAL_CLIENT_GAZEBO_H_

#include <string>
#include <vector>

//#include "Reveal/client/simulator.h"
#include "Reveal/core/ipc.h"
#include "Reveal/client/client.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

class gazebo_c : public simulator_c {
public:
  enum dynamics_e {
    DYNAMICS_ODE,
    DYNAMICS_BULLET,
    DYNAMICS_DART,
    DYNAMICS_SIMBODY
  };

  dynamics_e dynamics;

  std::string world_path;
  std::string model_path;
  std::string plugin_path;

  Reveal::Client::client_ptr _client;

  gazebo_c( void );
  gazebo_c( Reveal::Client::client_ptr client );
  virtual ~gazebo_c( void );

  bool execute( void );
  dynamics_e prompt_dynamics( void );
  void select_dynamics( void );

  std::vector< std::string > environment_keys( void );

  Reveal::Core::pipe_ptr ipc; 
  Reveal::Core::pipe_ptr exit_write;
  Reveal::Core::pipe_ptr exit_read;

  bool child_quit;
  void child_sighandler( int signum );
  void print_tuning_menu( void ); 

  virtual bool experiment( Reveal::Core::authorization_ptr auth );

  static void exit_sighandler( int signum );
};


//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::string package_root_path( void );
//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_GAZEBO_H_
