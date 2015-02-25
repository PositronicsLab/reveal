#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include "models.h"

//-----------------------------------------------------------------------------
static const double PERIOD = 5.0;
static const double AMP = 0.5;
static const double SMALL_AMP = AMP * 0.1;

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
      _arm->shoulder_pan_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD);
      _arm->shoulder_lift_actuator()->SetVelocity(0, std::cos(0)*SMALL_AMP*PERIOD*2.0);
      _arm->elbow_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*2.0/3.0);
      _arm->wrist1_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*1.0/7.0);
      _arm->wrist2_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*2.0/11.0);
      _arm->wrist3_actuator()->SetVelocity(0, std::cos(0)*AMP*PERIOD*3.0/13.0);
  
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
      control( t, q, qd, u );

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

    void control( double t, std::map<std::string,double> pos, std::map<std::string,double> vel, std::map<std::string,double>& force ) {
      // map in the state
      double sh_pan_q = pos.find( "shoulder_pan_joint" )->second;
      double sh_pan_qd = vel.find( "shoulder_pan_joint" )->second;
      double sh_lift_q = pos.find( "shoulder_lift_joint" )->second;
      double sh_lift_qd = vel.find( "shoulder_lift_joint" )->second;
      double elbow_q = pos.find( "elbow_joint" )->second;
      double elbow_qd = vel.find( "elbow_joint" )->second;
      double wrist1_q = pos.find( "wrist_1_joint" )->second;
      double wrist1_qd = vel.find( "wrist_1_joint" )->second;
      double wrist2_q = pos.find( "wrist_2_joint" )->second;
      double wrist2_qd = vel.find( "wrist_2_joint" )->second;
      double wrist3_q = pos.find( "wrist_3_joint" )->second;
      double wrist3_qd = vel.find( "wrist_3_joint" )->second;

      // determine the desired position and velocity for the controller 
      double sh_pan_q_des = std::sin(t) * AMP * PERIOD;
      double sh_pan_qd_des = std::cos(t) * AMP * PERIOD;
      double sh_lift_q_des = std::sin(t*2.0) * SMALL_AMP * PERIOD * 2.0;
      double sh_lift_qd_des = std::cos(t*2.0) * SMALL_AMP * PERIOD * 2.0;
      double elbow_q_des = std::sin(t*2.0/3.0) * AMP * PERIOD * 2.0/3.0;
      double elbow_qd_des = std::cos(t*2.0/3.0) * AMP * PERIOD * 2.0/3.0;
      double wrist1_q_des = std::sin(t*1.0/7.0) * AMP * PERIOD * 1.0/7.0;
      double wrist1_qd_des = std::cos(t*1.0/7.0) * AMP * PERIOD * 1.0/7.0;
      double wrist2_q_des = std::sin(t*2.0/11.0) * AMP * PERIOD * 2.0/11.0;
      double wrist2_qd_des = std::cos(t*2.0/11.0) * AMP * PERIOD * 2.0/11.0;
      double wrist3_q_des = std::sin(t*3.0/13.0) * AMP * PERIOD * 3.0/13.0;
      double wrist3_qd_des = std::cos(t*3.0/13.0) * AMP * PERIOD * 3.0/13.0;
  
      // compute the errors
      double sh_pan_q_err = sh_pan_q_des - sh_pan_q;
      double sh_pan_qd_err = sh_pan_qd_des - sh_pan_qd;
      double sh_lift_q_err = sh_lift_q_des - sh_lift_q;
      double sh_lift_qd_err = sh_lift_qd_des - sh_lift_qd;
      double elbow_q_err = elbow_q_des - elbow_q;
      double elbow_qd_err = elbow_qd_des - elbow_qd;
      double wrist1_q_err = wrist1_q_des - wrist1_q;
      double wrist1_qd_err = wrist1_qd_des - wrist1_qd;
      double wrist2_q_err = wrist2_q_des - wrist2_q;
      double wrist2_qd_err = wrist2_qd_des - wrist2_qd;
      double wrist3_q_err = wrist3_q_des - wrist3_q;
      double wrist3_qd_err = wrist3_qd_des - wrist3_qd;
  
      // setup gains
      const double SH_KP = 300.0, SH_KV = 120.0;
      const double EL_KP = 60.0, EL_KV = 24.0;
      const double WR_KP = 15.0, WR_KV = 6.0;
   
      // compute the actuator forces
      double sh_pan_f = SH_KP * sh_pan_q_err + SH_KV * sh_pan_qd_err;
      double sh_lift_f = SH_KP * sh_lift_q_err + SH_KV * sh_lift_qd_err;
      double elbow_f = EL_KP * elbow_q_err + EL_KV * elbow_qd_err;
      double wrist1_f = WR_KP * wrist1_q_err + WR_KV * wrist1_qd_err;
      double wrist2_f = WR_KP * wrist2_q_err + WR_KV * wrist2_qd_err;
      double wrist3_f = WR_KP * wrist3_q_err + WR_KV * wrist3_qd_err;
  
      // close fingers forces
      double finger_l_f =  100;
      double finger_r_f = -100;
      
      // map out the forces
      force.insert( std::pair<std::string,double>("shoulder_pan_joint", sh_pan_f ) );
      force.insert( std::pair<std::string,double>("shoulder_lift_joint", sh_lift_f ) );
      force.insert( std::pair<std::string,double>("elbow_joint", elbow_f ) );
      force.insert( std::pair<std::string,double>("wrist_1_joint", wrist1_f ) );
      force.insert( std::pair<std::string,double>("wrist_2_joint", wrist2_f ) );
      force.insert( std::pair<std::string,double>("wrist_3_joint", wrist3_f ) );
      force.insert( std::pair<std::string,double>("l_finger_actuator", finger_l_f ) );
      force.insert( std::pair<std::string,double>("r_finger_actuator", finger_r_f ) ); 
    }
 
  };

  GZ_REGISTER_MODEL_PLUGIN( arm_controller_c )

} // namespace gazebo

//-----------------------------------------------------------------------------

