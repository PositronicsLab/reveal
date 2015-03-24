#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include <Reveal/core/ipc.h>
#include <Reveal/core/pointers.h>
#include <Reveal/core/experiment.h>
#include <Reveal/core/scenario.h>
#include <Reveal/core/trial.h>
#include <Reveal/core/solution.h>
#include <Reveal/core/transport_exchange.h>

#include <Reveal/sim/gazebo/helpers.h>

//-----------------------------------------------------------------------------
namespace gazebo
{
  class world_plugin_c : public WorldPlugin
  {
  private:

    event::ConnectionPtr _preupdateConnection;
    event::ConnectionPtr _postupdateConnection;

    physics::WorldPtr _world;

    common::Time start_time, last_time;
    common::Time sim_time;

    Reveal::Core::authorization_ptr auth;
    Reveal::Core::scenario_ptr scenario;
    Reveal::Core::experiment_ptr experiment;
    Reveal::Core::trial_ptr trial;
    Reveal::Core::solution_ptr solution;

  private:
    Reveal::Core::pipe_ptr _revealpipe;

    int steps_this_trial;

    //-------------------------------------------------------------------------
//--- monitor
    bool connect( void ) {
      //TODO: correct constructor
      printf( "Connecting to server...\n" );

      // TODO: For now, these hardcoded values are okay, but most likely needs 
      // to be configurable through defines in cmake
      unsigned port = MONITOR_PORT;
      std::string host = "localhost";
      _revealpipe = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( host, port ) );
      if( _revealpipe->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
        return false;
      }

      printf( "Connected\n" );
      return true;
    }
//---

  public:

    //-------------------------------------------------------------------------
    world_plugin_c( ) {

    }

    //-------------------------------------------------------------------------
    ~world_plugin_c( ) {
      event::Events::DisconnectWorldUpdateBegin( _preupdateConnection );
      event::Events::DisconnectWorldUpdateBegin( _postupdateConnection );
      _revealpipe->close();
    }

  protected:

    //-------------------------------------------------------------------------
    void Load(physics::WorldPtr world, sdf::ElementPtr sdf) {
      std::cerr << "Starting World Plugin" << std::endl;
      std::cerr << "Connecting to Reveal Client..." << std::endl;

      if( !connect() ) {
        std::cerr << "Failed to connect to Reveal Client\nExiting\n" << std::endl;
        exit( 1 );
      }
      std::cerr << "Connected to Reveal Client." << std::endl;

//--- monitor
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

//---
      _preupdateConnection = event::Events::ConnectWorldUpdateBegin(
          boost::bind( &world_plugin_c::Preupdate, this ) );
      _postupdateConnection = event::Events::ConnectWorldUpdateEnd(
          boost::bind( &world_plugin_c::Postupdate, this ) );

      _world = world;
      Reveal::Sim::Gazebo::helpers_c::sim_time( 0.0, _world );

      // If validation added back in, do validation here

      // reset the world before we begin
      Reveal::Sim::Gazebo::helpers_c::reset( _world );

      steps_this_trial = 0;
    }

    //-------------------------------------------------------------------------
    void Preupdate( ) {
      //printf( "preupdate called\n" );

      // if in the middle of evaluating a trial, get out
      if( steps_this_trial > 0 ) return;

      // otherwise, read next trial state message from reveal client (blocking) 
//--- monitor
      Reveal::Core::transport_exchange_c ex;
      Reveal::Core::transport_exchange_c::error_e ex_err;
      std::string msg;

      if( _revealpipe->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
        // TODO: error recovery
      }
      ex_err = ex.parse( msg );
      if( ex_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
        //TODO: error handling
      }
      trial = ex.get_trial();
//---
      // set the simulation state from the trial
      Reveal::Sim::Gazebo::helpers_c::write_trial( trial, _world );
    }

    //-------------------------------------------------------------------------
    void Postupdate( ) {
      //printf( "postupdate called\n" );

      Reveal::Core::transport_exchange_c ex;
      Reveal::Core::transport_exchange_c::error_e ex_err;
      std::string msg;

      // if reached the end of the set of steps in the trial
      if( steps_this_trial == experiment->steps_per_trial ) {

        // get the solution from the simulator
        double t = Reveal::Sim::Gazebo::helpers_c::sim_time( _world );
        solution = scenario->get_solution( Reveal::Core::solution_c::CLIENT, trial, t );

        //_world->extract_solution( trial, solution );
        std::vector<std::string> model_list;
        model_list.push_back( "ur10_schunk_arm" );
        model_list.push_back( "block" );

        solution = Reveal::Sim::Gazebo::helpers_c::read_client_solution( _world, model_list, trial->scenario_id, trial->trial_id );

//--- monitor
        // and broadcast the solution to the reveal client.
        ex_err = ex.build_client_solution( msg, auth, experiment, solution );
        if( ex_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
          //TODO: error handling
        }
        if( _revealpipe->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          //TODO: error handling
        }
//---
        // if last trial, then exit gazebo
        if( trial->trial_id == scenario->trials - 1 ) exit( 0 );

        // otherwise, we need to reset the trial state, so reset the steps
        steps_this_trial = 0;

      } else {
        // otherwise advance to the next step
        steps_this_trial++;
      }
    }
  };

  GZ_REGISTER_WORLD_PLUGIN( world_plugin_c )

} // namespace gazebo


