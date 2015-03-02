#include <stdio.h>

#include <Reveal/analytics/types.h>
#include <Reveal/core/pointers.h>
#include <Reveal/core/solution_set.h>
#include <Reveal/core/solution.h>
#include <Reveal/core/scenario.h>
#include <Reveal/core/trial.h>

#include <Ravelin/Origin3d.h>
#include <Ravelin/Vector3d.h>
#include <Ravelin/Quatd.h>
#include <Ravelin/Matrix3d.h>
#include <Ravelin/Pose3d.h>

//-----------------------------------------------------------------------------

using namespace Reveal::Analytics;
//using namespace gazebo;

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
// Ravelin math
//-------------------------------------------------------------------------
Ravelin::Origin3d position( const Reveal::Core::state_c& state ) {
  return Ravelin::Origin3d( state[0], state[1], state[2] );

}

//-------------------------------------------------------------------------
Ravelin::Quatd rotation( const Reveal::Core::state_c& state ) {
  return Ravelin::Quatd( state[3], state[4], state[5], state[6] );

}

//-------------------------------------------------------------------------
Ravelin::Origin3d linear_velocity( const Reveal::Core::state_c& state ) {
  return Ravelin::Origin3d( state[7], state[8], state[9] );

}

//-------------------------------------------------------------------------
Ravelin::Origin3d angular_velocity( const Reveal::Core::state_c& state ) {
  return Ravelin::Origin3d( state[10], state[11], state[12] );

}

//-------------------------------------------------------------------------
// This function is in Ravelin, so can eliminate the redundancy
Ravelin::Origin3d to_omega( const Ravelin::Quatd& q, const Ravelin::Quatd& qd ) {
  Ravelin::Vector3d omega = Ravelin::Quatd::to_omega( q, qd ); 
  return Ravelin::Origin3d( omega.data() );
}

//-------------------------------------------------------------------------
// This function is in Ravelin, so can eliminate the redundancy
Ravelin::Quatd deriv( const Ravelin::Quatd& q, const Ravelin::Origin3d& w ) {
  Ravelin::Vector3d x = Ravelin::Vector3d( w.data() );
  return Ravelin::Quatd::deriv( q, x );
}

//-------------------------------------------------------------------------
double energy( double mass, Ravelin::Matrix3d I_tensor, double dt, Ravelin::Pose3d current_pose, Ravelin::Pose3d desired_pose, Ravelin::Origin3d current_linvel, Ravelin::Origin3d desired_linvel, Ravelin::Origin3d current_angvel, Ravelin::Origin3d desired_angvel ) {

  Ravelin::Origin3d x = current_pose.x;
  Ravelin::Origin3d x_star = desired_pose.x;
  Ravelin::Origin3d xd = current_linvel;
  Ravelin::Origin3d xd_star = desired_linvel;
  Ravelin::Quatd q = current_pose.q;
  Ravelin::Quatd q_star = desired_pose.q;

  Ravelin::Origin3d thetad = current_angvel;
  Ravelin::Origin3d thetad_star = desired_angvel;

  /*
  std::cout << "mass:" << mass << std::endl;
  std::cout << "I:" << I_tensor << std::endl;
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

  Ravelin::Origin3d v;
  Ravelin::Origin3d omega;

  Ravelin::Quatd qd = (q_star - q) * (1.0 / dt);
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

//  Ravelin::Origin3d o_omega = Ravelin::Origin3d( omega.data() );
//  Ravelin::Origin3d or_omega = I_tensor.mult( omega );
//  Ravelin::Origin3d r_omega = Ravelin::Origin3d( or_omega.data() );

  //                 linear          +        angular
  double KE = (0.5 * v.dot( v ) * mass) + (0.5 * omega.dot( I_tensor.mult( omega ) ));
  //  std::cout << "KE:" << KE << std::endl;
  return KE;
}
//-------------------------------------------------------------------------
double energy( Reveal::Core::link_ptr gripper, Reveal::Core::link_ptr target, Ravelin::Origin3d c_v, Ravelin::Origin3d c_omega, double mass, Ravelin::Matrix3d I, double dt ) {

  Ravelin::Origin3d target_pos = position( target->state );
  Ravelin::Quatd target_rot = rotation( target->state );
  Ravelin::Origin3d target_lvel = linear_velocity( target->state );
  Ravelin::Origin3d target_avel = angular_velocity( target->state );

  Ravelin::Origin3d gripper_pos = position( gripper->state );
  Ravelin::Quatd gripper_rot = rotation( gripper->state );
  Ravelin::Origin3d gripper_lvel = linear_velocity( gripper->state );
  Ravelin::Origin3d gripper_avel = angular_velocity( gripper->state );

  target_rot.normalize();
  gripper_rot.normalize();

  Ravelin::Pose3d x( target_rot, target_pos );

  //desired pose
  Ravelin::Pose3d x_des;

  Ravelin::Origin3d delta = gripper_pos - target_pos;
  Ravelin::Pose3d rot( target_rot );
  Ravelin::Vector3d _v = Ravelin::Vector3d( c_v.data() );
  Ravelin::Vector3d vrot = rot.transform_vector( _v );
  Ravelin::Origin3d v = Ravelin::Origin3d( vrot.data() );

  //Ravelin::Origin3d v = x.rot.RotateVector( c_v );   // legacy gazebo
  x_des.x = target_pos + (delta - v);

  Ravelin::Quatd dq = deriv( target_rot, c_omega );
  x_des.q = target_rot + dq;
  x_des.q.normalize();

  return energy( mass, I, dt, x, x_des, target_lvel, gripper_lvel, target_avel, gripper_avel );
}

//-------------------------------------------------------------------------
void get_initial_config( Reveal::Core::model_ptr arm, Reveal::Core::model_ptr target, Ravelin::Origin3d& c_v_l, Ravelin::Origin3d& c_v_r, Ravelin::Origin3d& c_omega_l, Ravelin::Origin3d& c_omega_r ) {

  Ravelin::Origin3d finger_l_pos, finger_r_pos, block_pos;
  Ravelin::Quatd finger_l_rot, finger_r_rot, block_rot;

  Reveal::Core::link_ptr finger_l = finger_l_link( arm );
  Reveal::Core::link_ptr finger_r = finger_r_link( arm );
  Reveal::Core::link_ptr block = block_link( target );
  assert( finger_l && finger_r && block );

  // initial desired velocities
  // left gripper energy constants
  c_v_l = position( finger_l->state ) - position( block->state );
  c_omega_l = to_omega( rotation( finger_l->state ), rotation( block->state ) );
  // right gripper energy constants
  c_v_r = position( finger_r->state ) - position( block->state );
  c_omega_r = to_omega( rotation( finger_r->state ), rotation( block->state ) );

}

//-------------------------------------------------------------------------
extern "C" {

//-------------------------------------------------------------------------
error_e analyze( Reveal::Core::solution_set_ptr input, Reveal::Core::analysis_ptr& output ) {

  Reveal::Core::model_ptr arm_t0, target_t0;

  // load model data for inertial properties.  This has to come from client
  // package or from other records inserted in response to client action.  
  // For now, this will be hard coded.

  // get the inertial properties of the box
  // this may need to be in a header for the experiment or read from the sdf
  double m = 0.12;
  Ravelin::Matrix3d I( 0.00001568, 0.0, 0.0, 
                   0.0, 0.00001568, 0.0, 
                   0.0, 0.0, 0.00001568 );

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

  printf( "trial0[%u]: ", trial0_idx );
  trial0->print();
  printf( "\n\n" );

  // get the initial state.  Drawn from the trial data.
  arm_t0 = arm_model( trial0 );
  target_t0 = target_model( trial0 );

  // left gripper energy constants
  Ravelin::Origin3d c_v_l;
  Ravelin::Origin3d c_omega_l;
  // right gripper energy constants
  Ravelin::Origin3d c_v_r;
  Ravelin::Origin3d c_omega_r;

  // compute the initial energy from initial state.
  get_initial_config( arm_t0, target_t0, c_v_l, c_v_r, c_omega_l, c_omega_r );

  // iterate over the client solutions and compute the energy for each sample
  for( unsigned i = 0; i < input->solutions.size(); i++ ) {
    Reveal::Core::solution_ptr solution = input->solutions[i];
    double t = solution->t;
    double dt = solution->dt;

    if( i == 0 ) {
      solution->print();
      printf( "\n\n" );
    }
    //std::vector<double> avgKEs;

    Reveal::Core::model_ptr arm = arm_model( solution );
    Reveal::Core::model_ptr target = target_model( solution );
    assert( arm && target );

    Reveal::Core::link_ptr finger_l = finger_l_link( arm );
    Reveal::Core::link_ptr finger_r = finger_r_link( arm );
    Reveal::Core::link_ptr block = block_link( target );
    assert( finger_l && finger_r && block );

    double KE_l = energy( finger_l, block, c_v_l, c_omega_l, m, I, dt );
    double KE_r = energy( finger_r, block, c_v_r, c_omega_r, m, I, dt );

    double avg_KE = (KE_l + KE_r) / 2.0;

    printf( "t[%f], avgKE[%f], KE_l[%f], KE_r[%f]\n", t, avg_KE, KE_l, KE_r );
  }
  return ERROR_NONE;
}

//-----------------------------------------------------------------------------

} // extern "C"

