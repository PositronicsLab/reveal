
#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include "models.h"

#include <sstream>

#define MAX_SIM_TIME 100.0

//-----------------------------------------------------------------------------
namespace gazebo 
{
  class arm_controller_c : public ModelPlugin
  {
  private:
    // the reference so that this ship is inserted into gazebo's callback system
    event::ConnectionPtr _updateConnection;
    // the gazebo reference to the world in which the ship is located
    physics::WorldPtr _world;

    arm_ptr _arm;
    target_ptr _target;

    // left gripper energy constants
    math::Vector3 _target_c_v_l;
    math::Vector3 _target_c_omega_l;
    // right gripper energy constants
    math::Vector3 _target_c_v_r;
    math::Vector3 _target_c_omega_r;

    //-------------------------------------------------------------------------
    static double energy( physics::LinkPtr gripper, physics::LinkPtr grip_target, math::Vector3 c_v, math::Vector3 c_omega, double target_mass, math::Matrix3 target_I_tensor, double dt ) {

      math::Pose x = grip_target->GetWorldPose();

      //desired pose
      math::Pose x_des;
      math::Vector3 delta = gripper->GetWorldPose().pos - grip_target->GetWorldPose().pos;
      c_v = x.rot.RotateVector( c_v );
      x_des.pos = x.pos + (delta - c_v);

      math::Quaternion dq = deriv( x.rot, c_omega );
      x_des.rot = x.rot + dq;
      x_des.rot.Normalize();

      math::Vector3 linvel = grip_target->GetWorldLinearVel();
      // desired linear velocity
      math::Vector3 linvel_des = gripper->GetWorldLinearVel();

      math::Vector3 angvel = grip_target->GetWorldAngularVel();   
      // desired angular velocity
      math::Vector3 angvel_des = gripper->GetWorldAngularVel();

      //if( !first_pass ) {
      double KE = energy( target_mass, target_I_tensor, dt, x, x_des, linvel, linvel_des, angvel, angvel_des );
      return KE;
    }

    //-------------------------------------------------------------------------
    static double energy( double mass, math::Matrix3 I_tensor, double dt, math::Pose current_pose, math::Pose desired_pose, math::Vector3 current_linvel, math::Vector3 desired_linvel, math::Vector3 current_angvel, math::Vector3 desired_angvel ) {

      math::Vector3 x = current_pose.pos;
      math::Vector3 x_star = desired_pose.pos;
      math::Vector3 xd = current_linvel;
      math::Vector3 xd_star = desired_linvel;
      math::Quaternion q = current_pose.rot;
      math::Quaternion q_star = desired_pose.rot;
  
      math::Vector3 thetad = current_angvel;
      math::Vector3 thetad_star = desired_angvel;
  
      /*
      std::cout << "m:" << m << std::endl;
      std::cout << "I:" << I << std::endl;
      std::cout << "dt:" << dt << std::endl;
      std::cout << "x:" << x << std::endl;
      std::cout << "x_star:" << x_star << std::endl;
      std::cout << "xd:" << xd << std::endl;
      std::cout << "xd_star:" << xd_star << std::endl;
      std::cout << "q:" << q << std::endl;
      std::cout << "q_star:" << q_star << std::endl;
      std::cout << "thetad:" << thetad << std::endl;
      std::cout << "thetad_star:" << thetad_star << std::endl;
      */
  
      math::Vector3 v;
      math::Vector3 omega;
  
      math::Quaternion qd = (q_star - q) * (1.0 / dt);
      //  std::cout << "qd:" << qd << std::endl;
  
      // assert qd not normalized
      const double EPSILON = 1e8;
      double mag = qd.x * qd.x + qd.y * qd.y + qd.z * qd.z + qd.w * qd.w;
      //std::cout << "mag:" << mag << std::endl;
      //assert( fabs( mag - 1.0 ) > EPSILON );
  
      v = (x_star - x) / dt + (xd_star - xd);
      //  std::cout << "v:" << v << std::endl;
      omega = to_omega( q, qd ) + (thetad_star - thetad);
      //  std::cout << "omega:" << omega << std::endl;
  
      //                 linear          +        angular
      double KE = (0.5 * v.Dot( v ) * mass) + (0.5 * omega.Dot( I_tensor * omega ));
      //  std::cout << "KE:" << KE << std::endl;
      return KE;
    }

    //-------------------------------------------------------------------------
    static math::Vector3 to_omega( math::Quaternion q, math::Quaternion qd ) {
      math::Vector3 omega;
      omega.x = 2 * (-q.x * qd.w + q.w * qd.x - q.z * qd.y + q.y * qd.z);
      omega.y = 2 * (-q.y * qd.w + q.z * qd.x + q.w * qd.y - q.x * qd.z);
      omega.z = 2 * (-q.z * qd.w - q.y * qd.x + q.x * qd.y + q.w * qd.z);
      return omega;
    }

    //-------------------------------------------------------------------------
    static math::Quaternion deriv( math::Quaternion q, math::Vector3 w) {
      math::Quaternion qd;

      qd.w = .5 * (-q.x * w.x - q.y * w.y - q.z * w.z); 
      qd.x = .5 * (+q.w * w.x + q.z * w.y - q.y * w.z);
      qd.y = .5 * (-q.z * w.x + q.w * w.y + q.x * w.z);
      qd.z = .5 * (+q.y * w.x - q.x * w.y + q.w * w.z);

      return qd;
    }

    //-------------------------------------------------------------------------
    double previous_t;
  public:
    //-------------------------------------------------------------------------
    arm_controller_c( void ) { }

    //-------------------------------------------------------------------------
    virtual ~arm_controller_c( void ) {
      event::Events::DisconnectWorldUpdateBegin( _updateConnection );
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called when the simulation is starting up
    virtual void Load( physics::ModelPtr model, sdf::ElementPtr sdf ) {
      // -- PRINCIPLE REFERENCES --
      _world = model->GetWorld();

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

      // reset the world before we begin
      _world->Reset();  
  
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
  
      // Get targets
      math::Vector3 c_v, c_omega;
      // left gripper energy constants
      c_v = _arm->finger_l()->GetWorldPose().pos - _target->link()->GetWorldPose().pos;
      _target_c_v_l = c_v;
      c_omega = to_omega( _arm->finger_l()->GetWorldPose().rot, _target->link()->GetWorldPose().rot );
      _target_c_omega_l = c_omega;
      // right gripper energy constants
      c_v = _arm->finger_r()->GetWorldPose().pos - _target->link()->GetWorldPose().pos;
      _target_c_v_r = c_v;
      c_omega = to_omega( _arm->finger_r()->GetWorldPose().rot, _target->link()->GetWorldPose().rot );
      _target_c_omega_r = c_omega;
      //printf( "found block target\n" );
  
      previous_t = 0.0;
  
      // -- FIN --
      printf( "arm_controller has initialized\n" );
  
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation advances a timestep
    virtual void Update( ) {
      // get the current time
      double t = _world->GetSimTime().Double();
      double dt = t - previous_t;
      double real_time = _world->GetRealTime().Double(); 
  
      //std::stringstream data;
      std::vector<double> avgKEs;
  
      //std::cout << "t : " << t << std::endl;
      //data << t << "," << real_time;
      // compute the energy of the target
  
      // get the inertial properties of the box
      physics::InertialPtr gz_inertial = _target->link()->GetInertial();
      double m = gz_inertial->GetMass();
      math::Vector3 pm = gz_inertial->GetPrincipalMoments();
      math::Matrix3 I( pm.x, 0.0, 0.0, 0.0, pm.y, 0.0, 0.0, 0.0, pm.z);
  
      double KE_l = energy( _arm->finger_l(), _target->link(), _target_c_v_l, _target_c_omega_l, m, I, dt );
      double KE_r = energy( _arm->finger_r(), _target->link(), _target_c_v_r, _target_c_omega_r, m, I, dt );
  
      double avg_KE = (KE_l + KE_r) / 2.0;
      //avgKEs.push_back( avg_KE );
  
      //std::cout << i << " KE[l,r,avg],pos : " << KE_l<< "," << KE_r << "," << avg_KE << "," << target->GetWorldPose().pos << std::endl;
      //data << "," << "0" << "," << KE_l << "," << KE_r << "," << avg_KE << "," << target->GetWorldPose().pos;
      
      //std::cout << "--------------------" << std::endl;
      //data << std::endl;
      //energy_log->write( data.str() );
  
      // check for exit condition
      if( fabs(avg_KE) > 1.0e7 ) {
        printf( "Kinetic Energy of the block exceeded 1e7... Killing sim.\n" );
        exit( 0 );
      }
      if( t >= MAX_SIM_TIME ) {
        printf( "Maximum simulation time exceeded... Killing sim\n" );
        exit( 0 );
      }
  
      // determinet the desired position and velocity for the controller 
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
  
      previous_t = t;

    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation is reset
    //virtual void Reset( ) { }

  };

  GZ_REGISTER_MODEL_PLUGIN( arm_controller_c )

} // namespace gazebo

//-----------------------------------------------------------------------------

