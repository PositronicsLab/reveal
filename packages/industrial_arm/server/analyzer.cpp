//#define ODEINT_V1                  // odeint versioning
//#define ODEINT_RUNGEKUTTA_STEPPER  // which stepper to use

//-----------------------------------------------------------------------------

#include <stdio.h>

//#include "pendulum.h"
#include <Reveal/types.h>
#include <Reveal/pointers.h>
#include <Reveal/solution_set.h>
#include <Reveal/solution.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>

//-----------------------------------------------------------------------------

using namespace Reveal::Analytics;

/* Begin - Analytics */
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
//TODO: fix due to changes from extraction
static void get_initial_config( void ) {
    /* Begin - Analytics */
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
    /* End - Analytics */

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
/* End - Analytics */

//-------------------------------------------------------------------------
extern "C" {

//-------------------------------------------------------------------------
error_e analyze( Reveal::Core::solution_set_ptr input, Reveal::Core::analysis_ptr& output ) {

      /* Begin - Analytics */
      // get the current time
/*
      double t = _world->sim_time();
      double dt = _world->step_size();
      double real_time = _world->real_time();
*/

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

      previous_t = t;
      /* End - Analytics */




/*
  //pendulum_c pendulum( EXPERIMENTAL_PENDULUM_L );


  printf( "Analyzed pendulum\n" );

  input->scenario->print();
  printf( "\n" );

  for( unsigned i = 0; i < input->trials.size(); i++ ) {
    Reveal::Core::trial_ptr trial = input->trials[i];
    printf( "trial [%u]: ", trial->trial_id );
    trial->print();
    printf( "\n" );

    Reveal::Core::solution_ptr solution = input->solutions[i];
    printf( "solution [%u]: ", solution->trial_id );
    solution->print();
    printf( "\n" );

    Reveal::Core::solution_ptr model = input->models[i];
    printf( "model [%u]: ", model->trial_id );
    model->print();
    printf( "\n\n" );
  }
*/
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

//Reveal::Analytics::error_e analyze( Reveal::Core::solution_set_ptr input, Reveal::Core::analysis_ptr& output );

} // extern "C"

