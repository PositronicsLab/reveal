#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include <Reveal/log.h>

#include "models.h"

#include <sstream>

//-----------------------------------------------------------------------------
namespace gazebo 
{
  class arm_controller_c : public ModelPlugin
  {
  private:
    // the reference so that this ship is inserted into gazebo's callback system
    event::ConnectionPtr _updateConnection;
    // the gazebo reference to the world in which the ship is located
    //physics::WorldPtr _world;
    world_ptr _world;

    arm_ptr _arm;
    target_ptr _target;

    // data generation only
    Reveal::Core::log_ptr _state_log;

  public:
    //-------------------------------------------------------------------------
    arm_controller_c( void ) { }

    //-------------------------------------------------------------------------
    virtual ~arm_controller_c( void ) {
      event::Events::DisconnectWorldUpdateBegin( _updateConnection );
 
      // -- DATA GENERATION ONLY --
      _state_log->close();
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called when the simulation is starting up
    virtual void Load( physics::ModelPtr model, sdf::ElementPtr sdf ) {
      // -- PRINCIPLE REFERENCES --
      //_world = model->GetWorld();

      std::string validation_errors;
      _world = world_ptr( new world_c( model->GetWorld() ) );
      if( !_world->validate( validation_errors ) ) {
        printf( "Unable to validate world in arm_controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      _arm = _world->arm();
      _target = _world->target();
/*
      // create the arm model encapsulation structure and validate it 
      _arm = arm_ptr( new arm_c( model ) );
      std::string validation_errors;
      if( !_arm->validate( validation_errors ) ) {
        printf( "Unable to validate industrial_arm model in arm_controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      // create the target model encapsulation structure and validate it 
      _target = target_ptr( new target_c( model->GetWorld() ) );
      if( !_target->validate( validation_errors ) ) {
        printf( "Unable to validate block model in arm_controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }
*/
      // reset the world before we begin
      //_world->Reset();  
      _world->reset();

  
      // set the starting velocity for the joints
      const double PERIOD = 5.0;
      const double AMP = 0.5;
      const double SMALL_AMP = AMP*0.1;
      _arm->shoulder_pan_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD);
      _arm->shoulder_lift_actuator()->SetVelocity(0, std::cos(0)*SMALL_AMP*PERIOD*2.0);
      _arm->elbow_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*2.0/3.0);
      _arm->wrist1_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*1.0/7.0);
      _arm->wrist2_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*2.0/11.0);
      _arm->wrist3_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*3.0/13.0);
  
      // -- CALLBACKS --
      _updateConnection = event::Events::ConnectWorldUpdateBegin(
        boost::bind( &arm_controller_c::Update, this ) );
 
      // -- DATA GENERATION ONLY --
      _state_log = Reveal::Core::log_ptr( new Reveal::Core::log_c( "state.log" ) );
      _state_log->open();
 
      // -- FIN --
      printf( "arm_controller has initialized\n" );
  
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation advances a timestep
    virtual void Update( ) {
      // get the current time
      //double t = _world->GetSimTime().Double();
      double t = _world->sim_time();

      // determine the desired position and velocity for the controller 
      const double PERIOD = 5.0;
      const double AMP = 0.5;
      const double SMALL_AMP = AMP*0.1;
      double sh_pan_q_des = std::sin(t)*AMP*PERIOD;
      double sh_pan_qd_des = std::cos(t)*AMP*PERIOD;
      double sh_lift_q_des = std::sin(t*2.0)*SMALL_AMP*PERIOD*2.0;
      double sh_lift_qd_des = std::cos(t*2.0)*SMALL_AMP*PERIOD*2.0;
      double elbow_q_des = std::sin(t*2.0/3.0)*AMP*PERIOD*2.0/3.0;
      double elbow_qd_des = std::cos(t*2.0/3.0)*AMP*PERIOD*2.0/3.0;
      double wrist1_q_des = std::sin(t*1.0/7.0)*AMP*PERIOD*1.0/7.0;
      double wrist1_qd_des = std::cos(t*1.0/7.0)*AMP*PERIOD*1.0/7.0;
      double wrist2_q_des = std::sin(t*2.0/11.0)*AMP*PERIOD*2.0/11.0;
      double wrist2_qd_des = std::cos(t*2.0/11.0)*AMP*PERIOD*2.0/11.0;
      double wrist3_q_des = std::sin(t*3.0/13.0)*AMP*PERIOD*3.0/13.0;
      double wrist3_qd_des = std::cos(t*3.0/13.0)*AMP*PERIOD*3.0/13.0;
  
      // compute the errors
      double sh_pan_q_err = (sh_pan_q_des - _arm->shoulder_pan_actuator()->GetAngle(0).Radian());
      double sh_pan_qd_err = (sh_pan_qd_des - _arm->shoulder_pan_actuator()->GetVelocity(0));
      double sh_lift_q_err = (sh_lift_q_des - _arm->shoulder_lift_actuator()->GetAngle(0).Radian());
      double sh_lift_qd_err = (sh_lift_qd_des - _arm->shoulder_lift_actuator()->GetVelocity(0));
      double elbow_q_err = (elbow_q_des - _arm->elbow_actuator()->GetAngle(0).Radian());
      double elbow_qd_err = (elbow_qd_des - _arm->elbow_actuator()->GetVelocity(0));
      double wrist1_q_err = (wrist1_q_des - _arm->wrist1_actuator()->GetAngle(0).Radian());
      double wrist1_qd_err = (wrist1_qd_des - _arm->wrist1_actuator()->GetVelocity(0));
      double wrist2_q_err = (wrist2_q_des - _arm->wrist2_actuator()->GetAngle(0).Radian());
      double wrist2_qd_err = (wrist2_qd_des - _arm->wrist2_actuator()->GetVelocity(0));
      double wrist3_q_err = (wrist3_q_des - _arm->wrist3_actuator()->GetAngle(0).Radian());
      double wrist3_qd_err = (wrist3_qd_des - _arm->wrist3_actuator()->GetVelocity(0));
  
      // setup gains
      const double SH_KP = 300.0, SH_KV = 120.0;
      const double EL_KP = 60.0, EL_KV = 24.0;
      const double WR_KP = 15.0, WR_KV = 6.0;
   
      // compute the actuator forces
      double sh_pan_f = SH_KP*sh_pan_q_err + SH_KV*sh_pan_qd_err;
      double sh_lift_f = SH_KP*sh_lift_q_err + SH_KV*sh_lift_qd_err;
      double elbow_f = EL_KP*elbow_q_err + EL_KV*elbow_qd_err;
      double wrist1_f = WR_KP*wrist1_q_err + WR_KV*wrist1_qd_err;
      double wrist2_f = WR_KP*wrist2_q_err + WR_KV*wrist2_qd_err;
      double wrist3_f = WR_KP*wrist3_q_err + WR_KV*wrist3_qd_err;
  
      // set the actuator forces for the arm
      _arm->shoulder_pan_actuator()->SetForce(0, sh_pan_f);
      _arm->shoulder_lift_actuator()->SetForce(0, sh_lift_f);
      _arm->elbow_actuator()->SetForce(0, elbow_f);
      _arm->wrist1_actuator()->SetForce(0, wrist1_f);
      _arm->wrist2_actuator()->SetForce(0, wrist2_f);
      _arm->wrist3_actuator()->SetForce(0, wrist3_f);
  
      // Simple close fingers test case
      _arm->finger_actuator_l()->SetForce(0, 100);  // close
      //_arm->finger_actuator_l()->SetForce(0, -0.00001);  // open
  
      _arm->finger_actuator_r()->SetForce(0,-100); // close
      //_arm->finger_actuator_r()->SetForce(0,0.00001);  //open
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation is reset
    //virtual void Reset( ) { }

  };

  GZ_REGISTER_MODEL_PLUGIN( arm_controller_c )

} // namespace gazebo

//-----------------------------------------------------------------------------

