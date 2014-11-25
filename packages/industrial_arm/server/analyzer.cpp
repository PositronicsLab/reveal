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

#include <gazebo/gazebo.hh>
#include <gazebo/math/gzmath.hh>

//-----------------------------------------------------------------------------

using namespace Reveal::Analytics;
using namespace gazebo;

// left gripper energy constants
static math::Vector3 _target_c_v_l;
static math::Vector3 _target_c_omega_l;
// right gripper energy constants
static math::Vector3 _target_c_v_r;
static math::Vector3 _target_c_omega_r;

//-------------------------------------------------------------------------
Reveal::Core::model_ptr arm_model( Reveal::Core::trial_ptr trial ) {
  Reveal::Core::model_ptr nothing;

  for( unsigned i = 0; i < trial->models.size(); i++ )
    if( trial->models[i]->id == "ur10_schunk_arm" ) return trial->models[i];

  return nothing;
}

//-------------------------------------------------------------------------
Reveal::Core::model_ptr target_model( Reveal::Core::trial_ptr trial ) {
  Reveal::Core::model_ptr nothing;

  for( unsigned i = 0; i < trial->models.size(); i++ )
    if( trial->models[i]->id == "block" ) return trial->models[i];

  return nothing;
}

//-------------------------------------------------------------------------
Reveal::Core::model_ptr arm_model( Reveal::Core::solution_ptr solution ) {
  Reveal::Core::model_ptr nothing;

  for( unsigned i = 0; i < solution->models.size(); i++ )
    if( solution->models[i]->id == "ur10_schunk_arm" ) return solution->models[i];

  return nothing;
}

//-------------------------------------------------------------------------
Reveal::Core::model_ptr target_model( Reveal::Core::solution_ptr solution ) {
  Reveal::Core::model_ptr nothing;

  for( unsigned i = 0; i < solution->models.size(); i++ )
    if( solution->models[i]->id == "block" ) return solution->models[i];

  return nothing;
}

//-------------------------------------------------------------------------
Reveal::Core::link_ptr finger_l_link( Reveal::Core::model_ptr model ) {
  Reveal::Core::link_ptr nothing;

  if( model->id != "ur10_schunk_arm" ) return nothing;
  for( unsigned i = 0; i < model->links.size(); i++ )
    if( model->links[i]->id == "schunk_mpg_80::l_finger" ) return model->links[i];

  return nothing;
}

//-------------------------------------------------------------------------
Reveal::Core::link_ptr finger_r_link( Reveal::Core::model_ptr model ) {
  Reveal::Core::link_ptr nothing;

  if( model->id != "ur10_schunk_arm" ) return nothing;
  for( unsigned i = 0; i < model->links.size(); i++ )
    if( model->links[i]->id == "schunk_mpg_80::r_finger" ) return model->links[i];

  return nothing;
}

//-------------------------------------------------------------------------
Reveal::Core::link_ptr block_link( Reveal::Core::model_ptr model ) {
  Reveal::Core::link_ptr nothing;

  if( model->id != "block" ) return nothing;
  for( unsigned i = 0; i < model->links.size(); i++ ) 
    if( model->links[i]->id == "body" ) return model->links[i];

  return nothing;
}

//-------------------------------------------------------------------------
math::Vector3 position( const Reveal::Core::state_c& state ) {
  return math::Vector3( state[0], state[1], state[2] );

}

//-------------------------------------------------------------------------
math::Quaternion rotation( const Reveal::Core::state_c& state ) {
  return math::Quaternion( state[3], state[4], state[5], state[6] );

}

//-------------------------------------------------------------------------
math::Vector3 linear_velocity( const Reveal::Core::state_c& state ) {
  return math::Vector3( state[7], state[8], state[9] );

}

//-------------------------------------------------------------------------
math::Vector3 angular_velocity( const Reveal::Core::state_c& state ) {
  return math::Vector3( state[10], state[11], state[12] );

}

/* Begin - Analytics */
//-------------------------------------------------------------------------
math::Vector3 to_omega( math::Quaternion q, math::Quaternion qd ) {
  math::Vector3 omega;
  omega.x = 2 * (-q.x * qd.w + q.w * qd.x - q.z * qd.y + q.y * qd.z);
  omega.y = 2 * (-q.y * qd.w + q.z * qd.x + q.w * qd.y - q.x * qd.z);
  omega.z = 2 * (-q.z * qd.w - q.y * qd.x + q.x * qd.y + q.w * qd.z);
  return omega;
}

//-------------------------------------------------------------------------
math::Quaternion deriv( math::Quaternion q, math::Vector3 w) {
  math::Quaternion qd;

  qd.w = .5 * (-q.x * w.x - q.y * w.y - q.z * w.z);
  qd.x = .5 * (+q.w * w.x + q.z * w.y - q.y * w.z);
  qd.y = .5 * (-q.z * w.x + q.w * w.y + q.x * w.z);
  qd.z = .5 * (+q.y * w.x - q.x * w.y + q.w * w.z);

  return qd;
}

//-------------------------------------------------------------------------
double energy( double mass, math::Matrix3 I_tensor, double dt, math::Pose current_pose, math::Pose desired_pose, math::Vector3 current_linvel, math::Vector3 desired_linvel, math::Vector3 current_angvel, math::Vector3 desired_angvel ) {

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
//double energy( physics::LinkPtr gripper, physics::LinkPtr grip_target, math::Vector3 c_v, math::Vector3 c_omega, double target_mass, math::Matrix3 target_I_tensor, double dt ) {

double energy( Reveal::Core::link_ptr gripper, Reveal::Core::link_ptr grip_target, math::Vector3 c_v, math::Vector3 c_omega, double target_mass, math::Matrix3 target_I_tensor, double dt ) {

  math::Vector3 target_pos = position( grip_target->state );
  math::Quaternion target_rot = rotation( grip_target->state );
  math::Vector3 target_lvel = linear_velocity( grip_target->state );
  math::Vector3 target_avel = angular_velocity( grip_target->state );

  math::Vector3 gripper_pos = position( gripper->state );
  math::Quaternion gripper_rot = rotation( gripper->state );
  math::Vector3 gripper_lvel = linear_velocity( gripper->state );
  math::Vector3 gripper_avel = angular_velocity( gripper->state );

  math::Pose x( target_pos, target_rot );

  //desired pose
  math::Pose x_des;

  math::Vector3 delta = gripper_pos - target_pos;
  c_v = target_rot.RotateVector( c_v );
  x_des.pos = target_pos + (delta - c_v);

  math::Quaternion dq = deriv( target_rot, c_omega );
  x_des.rot = target_rot + dq;
  x_des.rot.Normalize();

  return energy( target_mass, target_I_tensor, dt, x, x_des, target_lvel, gripper_lvel, target_avel, gripper_avel );
}

//-------------------------------------------------------------------------
//TODO: fix due to changes from extraction
void get_initial_config( Reveal::Core::model_ptr arm, Reveal::Core::model_ptr target ) {
  math::Vector3 finger_l_pos, finger_r_pos, block_pos;
  math::Quaternion finger_l_rot, finger_r_rot, block_rot;

  Reveal::Core::link_ptr finger_l = finger_l_link( arm );
  Reveal::Core::link_ptr finger_r = finger_r_link( arm );
  Reveal::Core::link_ptr block = block_link( target );
  assert( finger_l && finger_r && block );

  // initial desired velocities
  // left gripper energy constants
  _target_c_v_l = position( finger_l->state ) - position( block->state );
  _target_c_omega_l = to_omega( rotation( finger_l->state ), rotation( block->state ) );
  // right gripper energy constants
  _target_c_v_r = position( finger_r->state ) - position( block->state );
  _target_c_omega_r = to_omega( rotation( finger_r->state ), rotation( block->state ) );

}

//-------------------------------------------------------------------------
extern "C" {

//-------------------------------------------------------------------------
error_e analyze( Reveal::Core::solution_set_ptr input, Reveal::Core::analysis_ptr& output ) {

  Reveal::Core::model_ptr arm_t0, target_t0;

  // load model data for inertial properties.  This has to come from client
  // package or from other records inserted in response to client action.  
  // For now, this will be hard coded.
/*
  // get the inertial properties of the box
  physics::InertialPtr gz_inertial = _target->link()->GetInertial();
  double m = gz_inertial->GetMass();
  math::Vector3 pm = gz_inertial->GetPrincipalMoments();
  math::Matrix3 I( pm.x, 0.0, 0.0, 0.0, pm.y, 0.0, 0.0, 0.0, pm.z);
*/
  // this may need to be in a header for the experiment or read from the sdf
  double m = 0.12;
  math::Matrix3 I( 0.00001568, 0.0, 0.0, 0.0, 0.00001568, 0.0, 0.0, 0.0, 0.00001568 );

  unsigned trial0_idx = 0;
  double min_t = std::numeric_limits<double>::infinity();
  Reveal::Core::trial_ptr trial0;
  for( unsigned i = 0; i < input->trials.size(); i++ ) {
    if( input->trials[i]->t <= min_t ) {
      min_t = input->trials[i]->t;
      trial0_idx = i;
      trial0 = input->trials[i];
    }
  }

  printf( "trial0[%u]\n", trial0_idx );

  // get the initial state.  Drawn from the trial data.
  arm_t0 = arm_model( trial0 );
  target_t0 = target_model( trial0 );

  // compute the initial energy from initial state.
  get_initial_config( arm_t0, target_t0 );

  // iterate over the client solutions and compute the energy for each sample
  for( unsigned i = 0; i < input->solutions.size(); i++ ) {
    Reveal::Core::solution_ptr solution = input->solutions[i];
    double t = solution->t;
    double dt = solution->dt;

    std::vector<double> avgKEs;

    Reveal::Core::model_ptr arm = arm_model( solution );
    Reveal::Core::model_ptr target = target_model( solution );
    assert( arm && target );

    Reveal::Core::link_ptr finger_l = finger_l_link( arm );
    Reveal::Core::link_ptr finger_r = finger_r_link( arm );
    Reveal::Core::link_ptr block = block_link( target );
    assert( finger_l && finger_r && block );

    double KE_l = energy( finger_l, block, _target_c_v_l, _target_c_omega_l, m, I, dt );
    double KE_r = energy( finger_r, block, _target_c_v_r, _target_c_omega_r, m, I, dt );

    double avg_KE = (KE_l + KE_r) / 2.0;

    printf( "t[%f], avgKE[%f]\n", t, avg_KE );
  }

      /* Begin - Analytics */
      // get the current time
/*
      double t = _world->sim_time();
      double dt = _world->step_size();
      double real_time = _world->real_time();
*/

      //std::stringstream data;
      //std::vector<double> avgKEs;

      //std::cout << "t : " << t << std::endl;
      //data << t << "," << real_time;
      // compute the energy of the target

      // get the inertial properties of the box
/*
      physics::InertialPtr gz_inertial = _target->link()->GetInertial();
      double m = gz_inertial->GetMass();
      math::Vector3 pm = gz_inertial->GetPrincipalMoments();
      math::Matrix3 I( pm.x, 0.0, 0.0, 0.0, pm.y, 0.0, 0.0, 0.0, pm.z);

      // this may need to be in a header for the experiment or read from the
      // sdf
      double m = 0.12;
      math::Matrix3 I( 0.00001568, 0.0, 0.0, 0.0, 0.00001568, 0.0, 0.0, 0.0, 0.00001568 );
*/
/*
      double KE_l = energy( _arm->finger_l(), _target->link(), _target_c_v_l, _target_c_omega_l, m, I, dt );
      double KE_r = energy( _arm->finger_r(), _target->link(), _target_c_v_r, _target_c_omega_r, m, I, dt );

      double avg_KE = (KE_l + KE_r) / 2.0;
      //avgKEs.push_back( avg_KE );
*/
      //std::cout << i << " KE[l,r,avg],pos : " << KE_l<< "," << KE_r << "," << avg_KE << "," << target->GetWorldPose().pos << std::endl;
      //data << "," << "0" << "," << KE_l << "," << KE_r << "," << avg_KE << "," << target->GetWorldPose().pos;

      //std::cout << "--------------------" << std::endl;
      //data << std::endl;
      //energy_log->write( data.str() );
/*
      // check for exit condition
      if( fabs(avg_KE) > 1.0e7 ) {
        printf( "Kinetic Energy of the block exceeded 1e7... Killing sim.\n" );
        exit( 0 );
      }
      if( t >= MAX_SIM_TIME ) {
        printf( "Maximum simulation time exceeded... Killing sim\n" );
        exit( 0 );
      }
*/
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

} // extern "C"

