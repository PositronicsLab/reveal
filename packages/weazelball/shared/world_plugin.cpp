#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include <Reveal/ipc.h>
#include <Reveal/pointers.h>
#include <Reveal/experiment.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>
#include <Reveal/transport_exchange.h>

#define REVEAL_SERVICE

#include "models.h"

#define MAX_SIM_TIME 100.0

typedef double Real;

//-----------------------------------------------------------------------------
namespace gazebo
{
  class world_plugin_c : public WorldPlugin
  {
  private:

    event::ConnectionPtr updateConnection;

    //physics::WorldPtr _world;

    world_ptr _world;
    weazelball_ptr _weazelball;

    common::Time start_time, last_time;
    common::Time sim_time;

    //bool first_trial;
    bool last_trial;

    Reveal::Core::authorization_ptr auth;
    Reveal::Core::scenario_ptr scenario;
    Reveal::Core::experiment_ptr experiment;
    Reveal::Core::trial_ptr trial;
    Reveal::Core::solution_ptr solution;

  private:
    Reveal::Core::pipe_ptr _revealpipe;

    //-------------------------------------------------------------------------
    bool connect( void ) {
      //TODO: correct constructor
      printf( "Connecting to server...\n" );

      // TODO: For now, these hardcoded values are okay, but most likely needs 
      // to be configurable through defines in cmake
      unsigned port = GAZEBO_PORT;
      std::string host = "localhost";
      _revealpipe = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( host, port ) );
      if( _revealpipe->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
        return false;
      }

      printf( "Connected\n" );
      return true;
    }

  public:

    //-------------------------------------------------------------------------
    world_plugin_c( ) {

    }

    //-------------------------------------------------------------------------
    ~world_plugin_c( ) {
      event::Events::DisconnectWorldUpdateBegin( this->updateConnection );
      _revealpipe->close();
    }

  protected:

    //-------------------------------------------------------------------------
    void Load(physics::WorldPtr world, sdf::ElementPtr sdf) {
      std::cerr << "Starting World Plugin" << std::endl;

      if( !connect() ) {
        std::cerr << "Failed to connect to Reveal Client\nExiting\n" << std::endl;
        exit( 1 );
      }
      std::cerr << "Connected to Reveal Client" << std::endl;


      // read experiment
      Reveal::Core::transport_exchange_c ex;
      std::string msg;
      // block waiting for a server msg
      if( _revealpipe->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
        // TODO: error recovery
      }

      Reveal::Core::transport_exchange_c::error_e ex_err;
      ex_err = ex.parse_server_experiment( msg, auth, scenario, experiment );
      //TODO trap error

      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          boost::bind( &world_plugin_c::Update, this ) );

      _world = world_ptr( new world_c( world ) );

      _world->sim_time( 0.0 );

      //first_trial = true;
      last_trial = false;

      std::string validation_errors;
      if( !_world->validate( validation_errors ) ) {
        printf( "Unable to validate world in world_plugin\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      _weazelball = _world->weazelball();

      // reset the world before we begin
      _world->reset();

    }

    //-------------------------------------------------------------------------
    void Update( ) {
      Reveal::Core::transport_exchange_c ex;
      Reveal::Core::transport_exchange_c::error_e ex_err;
      std::string msg;

      static bool first_trial = true;
      static int steps_this_trial = 0;

      double t = _world->sim_time();
      double dt = _world->step_size();
      double real_time = _world->real_time();

      //printf( "experiment: " );
      //experiment->print();
      //printf( " trial_step[%d]", steps_this_trial );
      //printf( "\n" );

      if( first_trial ) {
        first_trial = false;
        // Note: have not taken a step yet that we want to record.
      } else {
        steps_this_trial++;
      }

      if( steps_this_trial == experiment->steps_per_trial ) {
        // build a solution
        solution = scenario->get_solution( Reveal::Core::solution_c::CLIENT, trial, t );

        _world->extract_solution( solution );

        ex_err = ex.build_client_solution( msg, auth, experiment, solution );
        if( ex_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
          //TODO: error handling
        }
        if( _revealpipe->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          //TODO: error handling
        }

        // since we just reset the world to trial state, reset the steps
        steps_this_trial = 0;

        // TODO: update last trial
        // if last trial, exit gazebo
        if( last_trial ) {
          exit( 0 );
        }
      }

      if( steps_this_trial == 0 ) {
        // read next trial state message from reveal client (block) 
        if( _revealpipe->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: error recovery
        }
        ex.parse( msg );
        trial = ex.get_trial();

        // if this is the last trial, flip the switch
        if( trial->trial_id == scenario->trials - 1 )
          last_trial = true;

        // set the simulation state from the trial
        _world->apply_trial( trial );

      }
    }

  };

  GZ_REGISTER_WORLD_PLUGIN( world_plugin_c )

} // namespace gazebo


