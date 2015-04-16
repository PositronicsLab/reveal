#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include <Reveal/core/scenario.h>
#include <Reveal/core/trial.h>
#include <Reveal/core/solution.h>

#include <Reveal/sim/gazebo/helpers.h>

// The DATA_GENERATION define is set by cmake and must be manually turned on
// so that the data is output to file.  This is an example of how the controller
// can be set up to produce data, but also be compatible for Reveal which by
// default has no knowledge of the cmake parameter so the switch is off when
// running a scenario via Reveal 
#ifdef DATA_GENERATION

// The DB_DIRECT_INSERT define is set by cmake and must be manually turned on so
// that data is directly inserted into the database.  This option can speed up
// development on a fully local platform as it circumvents the need to export
// then import, but it is not compatible to distributed Reveal client/server so
// should not be included in any form in distributed packages
#ifdef DB_DIRECT_INSERT
#ifndef _REVEAL_SERVER_SERVER_H_
#define _REVEAL_SERVER_SERVER_H_
#endif // _REVEAL_SERVER_SERVER_H_
#include <Reveal/db/database.h>
#endif  // DB_DIRECT_INSERT

#include <Reveal/core/exporter.h>
#include <Reveal/core/analyzer.h>
#endif // DATA_GENERATION

#include "arm_controller.h"

//-----------------------------------------------------------------------------
namespace gazebo 
{
  class arm_controller_c : public ModelPlugin
  {
  private:
    event::ConnectionPtr _preupdateConnection;
    event::ConnectionPtr _postupdateConnection;

    physics::WorldPtr _world;
    physics::ModelPtr _model;

    Reveal::Core::scenario_ptr _scenario;
    Reveal::Core::trial_ptr _trial;
    Reveal::Core::solution_ptr _solution;

    unsigned _trial_index;

    physics::LinkPtr _base; 
    physics::LinkPtr _shoulder;
    physics::LinkPtr _upperarm;
    physics::LinkPtr _forearm;
    physics::LinkPtr _wrist1;
    physics::LinkPtr _wrist2;
    physics::LinkPtr _wrist3;
  
    physics::JointPtr _shoulder_pan_actuator;   // shoulder to base
    physics::JointPtr _shoulder_lift_actuator;  // upperarm to shoulder
    physics::JointPtr _elbow_actuator;          // forearm to upperarm
    physics::JointPtr _wrist1_actuator;         // wrist1 to forearm
    physics::JointPtr _wrist2_actuator;         // wrist2 to wrist1
    physics::JointPtr _wrist3_actuator;         // wrist3 to wrist2

    // Schunk hand
    physics::LinkPtr _hand;
    physics::LinkPtr _finger_l;
    physics::LinkPtr _finger_r;

    physics::JointPtr _finger_actuator_l;
    physics::JointPtr _finger_actuator_r;

    bool _first_iteration;

#ifdef DATA_GENERATION
#ifdef DB_DIRECT_INSERT
    boost::shared_ptr<Reveal::DB::database_c> _db;
#endif // DB_DIRECT_INSERT
    Reveal::Core::datawriter_c _trial_datawriter;
    Reveal::Core::datawriter_c _solution_datawriter;
    Reveal::Core::exporter_c exporter;
#endif // DATA_GENERATION

  public:
    //-------------------------------------------------------------------------
    arm_controller_c( void ) { }

    //-------------------------------------------------------------------------
    virtual ~arm_controller_c( void ) {
      event::Events::DisconnectWorldUpdateBegin( _preupdateConnection );
      event::Events::DisconnectWorldUpdateBegin( _postupdateConnection );
 
#ifdef DB_DIRECT_INSERT
      _db->close();
#endif // DB_DIRECT_INSERT
    }

    bool validate( void ) {
      // ur10 arm
      _base = _model->GetLink( "ur10::base_link" );
      _shoulder = _model->GetLink( "ur10::shoulder_link" );
      _upperarm = _model->GetLink( "ur10::upper_arm_link" );
      _forearm = _model->GetLink( "ur10::forearm_link" );
      _wrist1 = _model->GetLink( "ur10::wrist_1_link" );
      _wrist2 = _model->GetLink( "ur10::wrist_2_link" );
      _wrist3 = _model->GetLink( "ur10::wrist_3_link" );
      _shoulder_pan_actuator = _model->GetJoint( "ur10::shoulder_pan_joint" );
      _shoulder_lift_actuator = _model->GetJoint( "ur10::shoulder_lift_joint" );
      _elbow_actuator = _model->GetJoint( "ur10::elbow_joint" );
      _wrist1_actuator = _model->GetJoint( "ur10::wrist_1_joint" );
      _wrist2_actuator = _model->GetJoint( "ur10::wrist_2_joint" );
      _wrist3_actuator = _model->GetJoint( "ur10::wrist_3_joint" );

      // schunk hand
      _hand = _model->GetLink( "schunk_mpg_80::base" );
      _finger_l = _model->GetLink( "schunk_mpg_80::l_finger" );
      _finger_r = _model->GetLink( "schunk_mpg_80::r_finger" );
      _finger_actuator_l = _model->GetJoint( "schunk_mpg_80::l_finger_actuator" );
      _finger_actuator_r = _model->GetJoint( "schunk_mpg_80::r_finger_actuator" );

      if( !( _world && _model && _base && _shoulder && _upperarm && _forearm &&
             _wrist1 && _wrist2 && _wrist3 && _shoulder_pan_actuator && 
             _shoulder_lift_actuator && _elbow_actuator && _wrist1_actuator && 
             _wrist2_actuator && _wrist3_actuator && _hand && _finger_l && 
             _finger_r && _finger_actuator_l && _finger_actuator_r ) )
        return false;
      return true;
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called when the simulation is starting up
    virtual void Load( physics::ModelPtr model, sdf::ElementPtr sdf ) {

      _first_iteration = true;

      _model = model;
      _world = model->GetWorld();
      if( !validate( ) ) {
        printf( "Unable to validate arm model in arm_controller\nERROR: Plugin failed to load\n" );
        return;
      }

      _trial_index = 0;

#ifdef DB_DIRECT_INSERT
      _db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c() );
      _db->open();

     // create and insert the scenario record
      _scenario = generate_scenario();
      _db->insert( _scenario );

      // create and insert the analyzer record
      Reveal::Core::analyzer_ptr analyzer = generate_analyzer( _scenario );
      _db->insert( analyzer );
#endif // DB_DIRECT_INSERT

      Reveal::Sim::Gazebo::helpers_c::reset( _world );

      // set the starting velocity for the joints
      std::map<std::string, double> qd;
      get_initial_velocity( qd );
 
      _shoulder_pan_actuator->SetVelocity(0, qd.find("shoulder_pan_joint")->second);
      _shoulder_lift_actuator->SetVelocity(0, qd.find("shoulder_lift_joint")->second);
      _elbow_actuator->SetVelocity(0, qd.find("elbow_joint")->second);
      _wrist1_actuator->SetVelocity(0, qd.find("wrist_1_joint")->second);
      _wrist2_actuator->SetVelocity(0, qd.find("wrist_2_joint")->second);
      _wrist3_actuator->SetVelocity(0, qd.find("wrist_3_joint")->second);

      // -- CALLBACKS --
      _preupdateConnection = event::Events::ConnectWorldUpdateBegin(
        boost::bind( &arm_controller_c::Preupdate, this ) );
      _postupdateConnection = event::Events::ConnectWorldUpdateEnd(
        boost::bind( &arm_controller_c::Postupdate, this ) );

#ifdef DATA_GENERATION

      _scenario = generate_scenario( );

      // write the initial trial.  State at t = 0 and no controls
//      _world->write_trial( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
      // TODO : rectify whether it is correct to write a trial here when a 
      // potential solution might not be found until post update
#endif // DATA_GENERATION

      // -- FIN --
      printf( "arm_controller has initialized\n" );
  
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation advances a timestep
    virtual void Preupdate( ) {

      // get the current time
      double t = Reveal::Sim::Gazebo::helpers_c::sim_time( _world );

      std::map<std::string, double> q;
      std::map<std::string, double> qd;
      std::map<std::string, double> u;

      // map in the positions
      q.insert( std::pair<std::string,double>("shoulder_pan_joint", _shoulder_pan_actuator->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("shoulder_lift_joint", _shoulder_lift_actuator->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("elbow_joint", _elbow_actuator->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("wrist_1_joint", _wrist1_actuator->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("wrist_2_joint", _wrist2_actuator->GetAngle(0).Radian() ) );
      q.insert( std::pair<std::string,double>("wrist_3_joint", _wrist3_actuator->GetAngle(0).Radian() ) );

      // map in the velocities
      qd.insert( std::pair<std::string,double>("shoulder_pan_joint", _shoulder_pan_actuator->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("shoulder_lift_joint", _shoulder_lift_actuator->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("elbow_joint", _elbow_actuator->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("wrist_1_joint", _wrist1_actuator->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("wrist_2_joint", _wrist2_actuator->GetVelocity(0) ) );
      qd.insert( std::pair<std::string,double>("wrist_3_joint", _wrist3_actuator->GetVelocity(0) ) );

      // compute controls
      get_control( t, q, qd, u );

      // apply the forces
      _shoulder_pan_actuator->SetForce(0, u.find("shoulder_pan_joint")->second);
      _shoulder_lift_actuator->SetForce(0, u.find("shoulder_lift_joint")->second);
      _elbow_actuator->SetForce(0, u.find("elbow_joint")->second);
      _wrist1_actuator->SetForce(0, u.find("wrist_1_joint")->second);
      _wrist2_actuator->SetForce(0, u.find("wrist_2_joint")->second);
      _wrist3_actuator->SetForce(0, u.find("wrist_3_joint")->second);
      _finger_actuator_l->SetForce(0, u.find("l_finger_actuator")->second);
      _finger_actuator_r->SetForce(0, u.find("r_finger_actuator")->second); 

#ifdef DATA_GENERATION

      _trial = generate_trial( _scenario, _trial_index,  
                           u.find("shoulder_pan_joint")->second, 
                           u.find("shoulder_lift_joint")->second, 
                           u.find("elbow_joint")->second, 
                           u.find("wrist_1_joint")->second,
                           u.find("wrist_2_joint")->second,
                           u.find("wrist_3_joint")->second,
                           u.find("l_finger_actuator")->second,
                           u.find("r_finger_actuator")->second  );
#ifdef DB_DIRECT_INSERT
      _db->insert( _trial );
#endif // DB_DIRECT_INSERT
#endif // DATA_GENERATION
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever called after integration
    virtual void Postupdate( ) {
#ifdef DATA_GENERATION
      double t = Reveal::Sim::Gazebo::helpers_c::sim_time( _world );
      double dt = Reveal::Sim::Gazebo::helpers_c::step_size( _world );

      std::vector<std::string> model_list;
      model_list.push_back( "ur10_schunk_arm" );
      model_list.push_back( "block" );

      _solution = Reveal::Sim::Gazebo::helpers_c::read_model_solution( _world, model_list, _scenario->id, _trial_index++ );
     
#ifdef DB_DIRECT_INSERT
      _db->insert( _solution );
#endif // DB_DIRECT_INSERT

      if( _first_iteration ) {
        Reveal::Core::analyzer_ptr analyzer = generate_analyzer( _scenario );
        bool result;
        result = exporter.write( _scenario, analyzer, _solution, _trial );
        _first_iteration = false;
      }

      exporter.write( t, dt, _trial );
      exporter.write( t, dt, _solution );
#endif // DATA_GENERATION
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation is reset
    //virtual void Reset( ) { }

#ifdef DATA_GENERATION
    //-------------------------------------------------------------------------
    // Data Generation methods.
    // For generating records directly from the simulation.  Not directly
    // used in Reveal implementations, but used to run the scenario and produce
    // data files that are compatible with Reveal import
    //-------------------------------------------------------------------------
    // Generates the scenario record data
    Reveal::Core::scenario_ptr generate_scenario( void ) {
      // create and insert the scenario record
      Reveal::Core::scenario_ptr scenario;
      scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

      scenario->id = "industrial_arm";
      scenario->description = "grasping a block with an industrial arm";
      // TODO : rectify determination of number of trials
      scenario->trials = 1000;  // NOTE: we don't know this in advance and we don't know when it will exit at this point!.
      // number of trials is arbitrary at this point
      scenario->steps_per_trial = 1;

      return scenario;
    }

    //-------------------------------------------------------------------------
    // Generates the analyzer record data
    Reveal::Core::analyzer_ptr generate_analyzer( Reveal::Core::scenario_ptr scenario ) {
      // create and insert the analyzer record
      Reveal::Core::analyzer_ptr analyzer;
      analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );
   
      analyzer->scenario_id = scenario->id;
      analyzer->filename = ANALYZER_PATH;
      analyzer->type = Reveal::Core::analyzer_c::PLUGIN;

      analyzer->keys.push_back( "t" );
      analyzer->labels.push_back( "Simulation time (s)" );

      analyzer->keys.push_back( "KE" );
      analyzer->labels.push_back( "Average Kinetic Energy of block" );

      return analyzer;
    }

    //-------------------------------------------------------------------------
    // Generates the trial record data
    Reveal::Core::trial_ptr generate_trial( Reveal::Core::scenario_ptr scenario, unsigned trial_index, double sh_pan_f, double sh_lift_f, double elbow_f, double wrist1_f, double wrist2_f, double wrist3_f, double finger_l_f, double finger_r_f ) {
      Reveal::Core::trial_ptr trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );
 
      trial->scenario_id = scenario->id;
      trial->trial_id = trial_index;
      trial->t = Reveal::Sim::Gazebo::helpers_c::sim_time( _world );
      trial->dt = Reveal::Sim::Gazebo::helpers_c::step_size( _world );

      std::map<std::string,double> arm_controls;
      arm_controls.insert( std::pair<std::string,double>( "ur10::shoulder_pan_joint", sh_pan_f ) );
      arm_controls.insert( std::pair<std::string,double>( "ur10::shoulder_lift_joint", sh_lift_f ) );
      arm_controls.insert( std::pair<std::string,double>( "ur10::elbow_joint", elbow_f ) );
      arm_controls.insert( std::pair<std::string,double>( "ur10::wrist_1_joint", wrist1_f ) );
      arm_controls.insert( std::pair<std::string,double>( "ur10::wrist2_joint", wrist2_f ) );
      arm_controls.insert( std::pair<std::string,double>( "ur10::wrist3_joint", wrist3_f ) );
      arm_controls.insert( std::pair<std::string,double>( "schunk_mpg_80::l_finger_actuator", finger_l_f ) );
      arm_controls.insert( std::pair<std::string,double>( "schunk_mpg_80::r_finger_actuator", finger_r_f ) );
      Reveal::Core::model_ptr arm_model = Reveal::Sim::Gazebo::helpers_c::read_model( _world, "ur10_schunk_arm", arm_controls );

      trial->models.push_back( arm_model );

      std::map<std::string,double> null_controls;
      Reveal::Core::model_ptr block_model = Reveal::Sim::Gazebo::helpers_c::read_model( _world, "block", null_controls );

      trial->models.push_back( block_model );
      
      return trial;
    }
#endif // DATA_GENERATION
  };

  GZ_REGISTER_MODEL_PLUGIN( arm_controller_c )

} // namespace gazebo

//-----------------------------------------------------------------------------

