#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include "models.h"

#include <sstream>

#define PI 3.14159265359

#define MOTOR_HZ 2.2

// will want to parameterize gains
#define MOTOR_CONTROLLER_KD 0.01

//-----------------------------------------------------------------------------
namespace gazebo 
{
  class controller_c : public ModelPlugin
  {
  private:
    event::ConnectionPtr _updateConnection;

    world_ptr _world;
    weazelball_ptr _weazelball;

  public:
    //-------------------------------------------------------------------------
    controller_c( void ) { }

    //-------------------------------------------------------------------------
    virtual ~controller_c( void ) {
      event::Events::DisconnectWorldUpdateBegin( _updateConnection );
 
      _world->close();
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called when the simulation is starting up
    virtual void Load( physics::ModelPtr model, sdf::ElementPtr sdf ) {

      std::string validation_errors;
      _world = world_ptr( new world_c( model->GetWorld() ) );
      if( !_world->validate( validation_errors ) ) {
        printf( "Unable to validate world in controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      // open the world before we begin
      _world->open();

      // get references to objects in the world
      _weazelball = _world->weazelball();
   
      // -- CALLBACKS --
      _updateConnection = event::Events::ConnectWorldUpdateBegin(
        boost::bind( &controller_c::Update, this ) );

#ifdef DATA_GENERATION
      //double desired_angpos;
      // write the initial trial.  State at t = 0 and no controls
      _world->write_trial( 0.0 );
#endif
 
      // -- FIN --
      printf( "controller has initialized\n" );
  
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation advances a timestep
    virtual void Update( ) {
#ifdef DATA_GENERATION
/*
      static bool first_trial = true;

      if( first_trial ) 
        first_trial = false;
      else 
        _world->write_solution();
*/
      _world->write_solution();
#endif

      // get the current time
      double t = _world->sim_time();
      double angvel = _weazelball->actuator()->GetVelocity( 0 );
      double desired_angvel = MOTOR_HZ * ( 2.0 * PI );

      // compute the actuator forces
      double f = MOTOR_CONTROLLER_KD * ( desired_angvel - angvel );

      // set the actuator forces for the weazelball
      _weazelball->actuator()->SetForce( 0, f );

#ifdef DATA_GENERATION
      _world->write_trial( f );
#endif
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation is reset
    //virtual void Reset( ) { }

  };

  GZ_REGISTER_MODEL_PLUGIN( controller_c )

} // namespace gazebo

//-----------------------------------------------------------------------------

