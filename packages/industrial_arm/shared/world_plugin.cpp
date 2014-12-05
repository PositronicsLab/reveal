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

#define REVEAL_SERVICE

#include "models.h"

#define MAX_SIM_TIME 100.0

typedef double Real;

//-----------------------------------------------------------------------------
namespace gazebo
{
  class world_plugin_c : public WorldPlugin
  {
  private:

    event::ConnectionPtr updateConnection;

    //physics::WorldPtr _world;

    world_ptr _world;
    arm_ptr _arm;
    target_ptr _target;

    common::Time start_time, last_time;
    common::Time sim_time;

    //bool first_trial;
    bool last_trial;

    Reveal::Core::authorization_ptr auth;
    Reveal::Core::scenario_ptr scenario;
    Reveal::Core::experiment_ptr experiment;
    Reveal::Core::trial_ptr trial;
    Reveal::Core::solution_ptr solution;

/*
    // Begin - Analytics
    // left gripper energy constants
    math::Vector3 _target_c_v_l;
    math::Vector3 _target_c_omega_l;
    // right gripper energy constants
    math::Vector3 _target_c_v_r;
    math::Vector3 _target_c_omega_r;

    double previous_t;
    // End - Analytics
*/

  private:
    Reveal::Core::pipe_ptr _revealpipe;

    //-------------------------------------------------------------------------
    bool connect( void ) {
      //TODO: correct constructor
      printf( "Connecting to server...\n" );

      // TODO: For now, these hardcoded values are okay, but most likely needs 
      // to be configurable through defines in cmake
      unsigned port = GAZEBO_PORT;
      std::string host = "localhost";
      _revealpipe = Reveal::Core::pipe_ptr( new Reveal::Core::pipe_c( host, port ) );
      if( _revealpipe->open() != Reveal::Core::pipe_c::ERROR_NONE ) {
        return false;
      }

      printf( "Connected\n" );
      return true;
    }
/*
    // Begin - Analytics 
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

      
      //std::cout << "m:" << m << std::endl;
      //std::cout << "I:" << I << std::endl;
      //std::cout << "dt:" << dt << std::endl;
      //std::cout << "x:" << x << std::endl;
      //std::cout << "x_star:" << x_star << std::endl;
      //std::cout << "xd:" << xd << std::endl;
      //std::cout << "xd_star:" << xd_star << std::endl;
      //std::cout << "q:" << q << std::endl;
      //std::cout << "q_star:" << q_star << std::endl;
      //std::cout << "thetad:" << thetad << std::endl;
      //std::cout << "thetad_star:" << thetad_star << std::endl;
      

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
    // End - Analytics
*/

  public:

    //-------------------------------------------------------------------------
    world_plugin_c( ) {

    }

    //-------------------------------------------------------------------------
    ~world_plugin_c( ) {
      event::Events::DisconnectWorldUpdateBegin( this->updateConnection );
      _revealpipe->close();
    }

  protected:

    //-------------------------------------------------------------------------
    void Load(physics::WorldPtr world, sdf::ElementPtr sdf) {
      std::cerr << "Starting World Plugin" << std::endl;

      if( !connect() ) {
        std::cerr << "Failed to connect to Reveal Client\nExiting\n" << std::endl;
        exit( 1 );
      }
      std::cerr << "Connected to Reveal Client" << std::endl;


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

      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          boost::bind( &world_plugin_c::Update, this ) );

      //_world = world;
      _world = world_ptr( new world_c( world ) );

      //start_time = world->GetSimTime();
      //last_time = start_time;
      //sim_time = common::Time( 0.0 );
      //_world->SetSimTime( sim_time );
      _world->sim_time( 0.0 );

      //first_trial = true;
      last_trial = false;

      std::string validation_errors;
      if( !_world->validate( validation_errors ) ) {
        printf( "Unable to validate world in world_plugin\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      _arm = _world->arm();
      _target = _world->target();
/*
      // create the arm model encapsulation structure and validate it 
      _arm = arm_ptr( new arm_c( world ) );
      std::string validation_errors;
      if( !_arm->validate( validation_errors ) ) {
        printf( "Unable to validate industrial_arm model in arm_controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      // create the target model encapsulation structure and validate it 
      _target = target_ptr( new target_c( world ) );
      if( !_target->validate( validation_errors ) ) {
        printf( "Unable to validate block model in arm_controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }
*/
      // reset the world before we begin
      //_world->Reset();
      _world->reset();

      // Get targets
/*
      // Begin - Analytics
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
      // End - Analytics
*/
    }

    //-------------------------------------------------------------------------
    void Update( ) {
      Reveal::Core::transport_exchange_c ex;
      Reveal::Core::transport_exchange_c::error_e ex_err;
      std::string msg;

      static bool first_trial = true;
      static int steps_this_trial = 0;

      double t = _world->sim_time();
      double dt = _world->step_size();
      double real_time = _world->real_time();

      //printf( "experiment: " );
      //experiment->print();
      //printf( " trial_step[%d]", steps_this_trial );
      //printf( "\n" );

      if( first_trial ) {
        first_trial = false;
        // Note: have not taken a step yet that we want to record.
      } else {
        steps_this_trial++;
      }

      if( steps_this_trial == experiment->steps_per_trial ) {
        // build a solution
        solution = scenario->get_solution( Reveal::Core::solution_c::CLIENT, trial, t );

        _world->extract_solution( solution );

        ex_err = ex.build_client_solution( msg, auth, experiment, solution );
        if( ex_err != Reveal::Core::transport_exchange_c::ERROR_NONE ) {
          //TODO: error handling
        }
        if( _revealpipe->write( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          //TODO: error handling
        }

        // since we just reset the world to trial state, reset the steps
        steps_this_trial = 0;

        // TODO: update last trial
        // if last trial, exit gazebo
        if( last_trial ) {
          exit( 0 );
        }
      }

      if( steps_this_trial == 0 ) {
        // read next trial state message from reveal client (block) 
        if( _revealpipe->read( msg ) != Reveal::Core::pipe_c::ERROR_NONE ) {
          // TODO: error recovery
        }
        ex.parse( msg );
        trial = ex.get_trial();

        // if this is the last trial, flip the switch
        if( trial->trial_id == scenario->trials - 1 )
          last_trial = true;

        // set the simulation state from the trial
        _world->apply_trial( trial );

      }
/*
      // Begin - Analytics
      // get the current time

//      double t = _world->sim_time();
//      double dt = _world->step_size();
//      double real_time = _world->real_time();


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
      // End - Analytics
*/
    }

  };

  GZ_REGISTER_WORLD_PLUGIN( world_plugin_c )

} // namespace gazebo


