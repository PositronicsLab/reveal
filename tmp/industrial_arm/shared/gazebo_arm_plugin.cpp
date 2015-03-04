#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include "models.h"
#include "arm_controller.h"

//-----------------------------------------------------------------------------
namespace gazebo 
{
  class arm_controller_c : public ModelPlugin
  {
  private:
    event::ConnectionPtr _preupdateConnection;
    event::ConnectionPtr _postupdateConnection;

    world_ptr _world;
    arm_ptr _arm;

  public:
    //-------------------------------------------------------------------------
    arm_controller_c( void ) { }

    //-------------------------------------------------------------------------
    virtual ~arm_controller_c( void ) {
      event::Events::DisconnectWorldUpdateBegin( _preupdateConnection );
      event::Events::DisconnectWorldUpdateBegin( _postupdateConnection );
 
      _world->close();
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called when the simulation is starting up
    virtual void Load( physics::ModelPtr model, sdf::ElementPtr sdf ) {

      std::string validation_errors;
      _world = world_ptr( new world_c( model->GetWorld() ) );
      if( !_world->validate( validation_errors ) ) {
        printf( "Unable to validate world in arm_controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      // open the world before we begin
      _world->open();

      // get references to objects in the world
      _arm = _world->arm();
  
      // set the starting velocity for the joints
      std::map<std::string, double> qd;
      get_initial_velocity( qd );

      _arm->shoulder_pan_actuator()->SetVelocity(0, qd.find("shoulder_pan_joint")->second);
      _arm->shoulder_lift_actuator()->SetVelocity(0, qd.find("shoulder_lift_joint")->second);
      _arm->elbow_actuator()->SetVelocity(0, qd.find("elbow_joint")->second);
      _arm->wrist1_actuator()->SetVelocity(0, qd.find("wrist_1_joint")->second);
      _arm->wrist2_actuator()->SetVelocity(0, qd.find("wrist_2_joint")->second);
      _arm->wrist3_actuator()->SetVelocity(0, qd.find("wrist_3_joint")->second);

      // -- CALLBACKS --
      _preupdateConnection = event::Events::ConnectWorldUpdateBegin(
        boost::bind( &arm_controller_c::Preupdate, this ) );
      _postupdateConnection = event::Events::ConnectWorldUpdateEnd(
        boost::bind( &arm_controller_c::Postupdate, this ) );

#ifdef DATA_GENERATION
      // write the initial trial.  State at t = 0 and no controls
//      _world->write_trial( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
      // TODO : rectify whether it is correct to write a trial here when a 
      // potential solution might not be found until post update
#endif

      // -- FIN --
      printf( "arm_controller has initialized\n" );
  
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation advances a timestep
    virtual void Preupdate( ) {

      // get the current time
      double t = _world->sim_time();

      //printf( "preupdate: simtime[%f]\n", t );

      std::map<std::string, double> q;
      std::map<std::string, double> qd;
      std::map<std::string, double> u;

      // map in the positions
      q.insert( std::pair<std::string,double>("shoulder_pan_joint", _arm->shoulder_pan_actuator()->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("shoulder_lift_joint", _arm->shoulder_lift_actuator()->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("elbow_joint", _arm->elbow_actuator()->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("wrist_1_joint", _arm->wrist1_actuator()->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("wrist_2_joint", _arm->wrist2_actuator()->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("wrist_3_joint", _arm->wrist3_actuator()->GetAngle(0).Radian() ) );

      // map in the velocities
      qd.insert( std::pair<std::string,double>("shoulder_pan_joint", _arm->shoulder_pan_actuator()->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("shoulder_lift_joint", _arm->shoulder_lift_actuator()->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("elbow_joint", _arm->elbow_actuator()->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("wrist_1_joint", _arm->wrist1_actuator()->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("wrist_2_joint", _arm->wrist2_actuator()->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("wrist_3_joint", _arm->wrist3_actuator()->GetVelocity(0) ) );

      // compute controls
      get_control( t, q, qd, u );

      // apply the forces
      _arm->shoulder_pan_actuator()->SetForce(0, u.find("shoulder_pan_joint")->second);
      _arm->shoulder_lift_actuator()->SetForce(0, u.find("shoulder_lift_joint")->second);
      _arm->elbow_actuator()->SetForce(0, u.find("elbow_joint")->second);
      _arm->wrist1_actuator()->SetForce(0, u.find("wrist_1_joint")->second);
      _arm->wrist2_actuator()->SetForce(0, u.find("wrist_2_joint")->second);
      _arm->wrist3_actuator()->SetForce(0, u.find("wrist_3_joint")->second);
      _arm->finger_actuator_l()->SetForce(0, u.find("l_finger_actuator")->second);
      _arm->finger_actuator_r()->SetForce(0, u.find("r_finger_actuator")->second); 

#ifdef DATA_GENERATION
      _world->write_trial( u.find("shoulder_pan_joint")->second, 
                           u.find("shoulder_lift_joint")->second, 
                           u.find("elbow_joint")->second, 
                           u.find("wrist_1_joint")->second,
                           u.find("wrist_2_joint")->second,
                           u.find("wrist_3_joint")->second,
                           u.find("l_finger_actuator")->second,
                           u.find("r_finger_actuator")->second );
#endif
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever called after integration
    virtual void Postupdate( ) {
#ifdef DATA_GENERATION
      double t = _world->sim_time();

      //printf( "postupdate: simtime[%f]\n", t );

      _world->write_solution();
#endif
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation is reset
    //virtual void Reset( ) { }
  };

  GZ_REGISTER_MODEL_PLUGIN( arm_controller_c )

} // namespace gazebo

//-----------------------------------------------------------------------------

