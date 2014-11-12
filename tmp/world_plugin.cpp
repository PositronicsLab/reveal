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

typedef double Real;

namespace gazebo
{
  class world_plugin_c : public WorldPlugin
  {
  private:

    event::ConnectionPtr updateConnection;

    physics::WorldPtr _world;

    common::Time start_time, last_time;
    common::Time sim_time;

    bool first_trial;
    bool last_trial;

    Reveal::Core::authorization_ptr auth;
    Reveal::Core::scenario_ptr scenario;
    Reveal::Core::experiment_ptr experiment;
    Reveal::Core::trial_ptr trial;
    Reveal::Core::solution_ptr solution;

  private:
    Reveal::Core::pipe_ptr _revealpipe;

    bool connect( void ) {
      //TODO: correct constructor
      printf( "Connecting to server...\n" );

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

    world_plugin_c( ) {

    }

    ~world_plugin_c( ) {
      event::Events::DisconnectWorldUpdateBegin( this->updateConnection );
      _revealpipe->close();
    }

  protected:

    void Load(physics::WorldPtr world, sdf::ElementPtr sdf) {
      std::cerr << "Starting World Plugin" << std::endl;

      if( !connect() ) {
        std::cerr << "Failed to connect to Reveal Client\nExiting\n" << std::endl;
        exit( 1 );
      }
      std::cerr << "Connected to Reveal Client" << std::endl;


      // read experiment
      Reveal::Core::transport_exchange_c exchange;
      std::string msg;
      // block waiting for a server msg
      if( _revealpipe->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
        // TODO: error recovery
      }
      exchange.parse( msg );
      auth = exchange.get_authorization();
      experiment = exchange.get_experiment();
      scenario = exchange.get_scenario(); 

      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          boost::bind( &world_plugin_c::Update, this ) );

      _world = world;

      //start_time = world->GetSimTime();
      //last_time = start_time;
      sim_time = common::Time( 0.0 );
      _world->SetSimTime( sim_time );

      first_trial = true;
      last_trial = false;
    }

    void Update( ) {
      // if not first pass, write the previous trial state to reveal client
      if( !first_trial ) {
        double t = _world->GetSimTime().Double();

        // build a solution
        solution = scenario->get_solution( Reveal::Core::solution_c::CLIENT, trial, t );

        // finish building the solution by inserting the state data
        //solution->state.append_q( 0.0 );
        //solution->state.append_dq( 0.0 );
        //solution->print();

        Reveal::Core::transport_exchange_c ex;
        Reveal::Core::transport_exchange_c::error_e ex_err;
        std::string msg;
        
        ex.set_authorization( auth );
        ex.set_origin( Reveal::Core::transport_exchange_c::ORIGIN_CLIENT );
        ex.set_type( Reveal::Core::transport_exchange_c::TYPE_SOLUTION );
        ex.set_experiment( experiment );
        ex.set_solution( solution );
        ex_err = ex.build( msg );
        if( ex_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
          //TODO: error handling
        }
        if( _revealpipe->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          //TODO: error handling
        }
     } else {
        first_trial = false;
      }

      // TODO: update last trial
      // if last trial, exit gazebo
      if( last_trial ) {
        exit( 0 );
      }

      // read next trial state message from reveal client (block) 
      std::string msg;
      Reveal::Core::transport_exchange_c ex;
      Reveal::Core::transport_exchange_c::error_e ex_err;
      
      if( _revealpipe->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
        // TODO: error recovery
      }
      ex.parse( msg );
      trial = ex.get_trial();

      // overwrite gazebo state based on reveal state message
        // read the time from the message and update
        // read the state of each robot from the message and map to the models
/*
       // **SIMULATION WOULD BE RUN HERE**
       double ti = trial->t;
       double dt = trial->dt;
       double tf = ti + dt;
*/



      
/* 
      double t = sim_time.Double() + 0.1;
      sim_time.Set( t );

      _world->SetSimTime( sim_time );

      t = _world->GetSimTime().Double();
      std::cerr << "World Plugin Callback: " << t << std::endl;

      common::Time sim_time = world->GetSimTime();

      last_time = sim_time;
*/

      if( trial->trial_id == scenario->trials - 1 )
        last_trial = true;
    }

  };

  GZ_REGISTER_WORLD_PLUGIN( world_plugin_c )

} // namespace gazebo


