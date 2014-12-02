#ifndef _REVEAL_SCENARIO_WEAZELBALL_MODELS_H_
#define _REVEAL_SCENARIO_WEAZELBALL_MODELS_H_

//-----------------------------------------------------------------------------
#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include <Reveal/state.h>

#define DATA_GENERATION

#ifdef DATA_GENERATION

#ifndef _REVEAL_SERVER_SERVER_H_
#define _REVEAL_SERVER_SERVER_H_
#endif

#include <Reveal/ipc.h>
#include <Reveal/pointers.h>
#include <Reveal/experiment.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>
#include <Reveal/transport_exchange.h>
#include <Reveal/model.h>
#include <Reveal/link.h>
#include <Reveal/joint.h>
#include <Reveal/database.h>
#endif

#ifdef REVEAL_SERVICE
#include <Reveal/ipc.h>
#include <Reveal/pointers.h>
#include <Reveal/experiment.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>
#include <Reveal/transport_exchange.h>
#include <Reveal/model.h>
#include <Reveal/link.h>
#include <Reveal/joint.h>
#endif

//-----------------------------------------------------------------------------
using namespace gazebo;

//-----------------------------------------------------------------------------

class model_c;
typedef boost::shared_ptr< model_c > model_ptr;

class weazelball_c;
typedef boost::shared_ptr< weazelball_c > weazelball_ptr;

class world_c;
typedef boost::shared_ptr< world_c > world_ptr;

//-----------------------------------------------------------------------------
class model_c
{
protected:
  physics::WorldPtr _world;
  physics::ModelPtr _model;

  physics::ModelPtr model( std::string name ) {
    return _world->GetModel( name );
  }

  physics::LinkPtr link( std::string name ) {
    return _model->GetLink( name );
  }

  physics::JointPtr joint( std::string name ) {
    return _model->GetJoint( name );
  }

  physics::ModelPtr model( std::string name, std::string& errors ) {
    physics::ModelPtr m = model( name );
    if( !m ) errors += "Unable to find model: " + name + "\n";
    return m;
  }

  physics::LinkPtr link( std::string name, std::string& errors ) {
    physics::LinkPtr l = link( name );
    if( !l ) errors += "Unable to find link: " + name + "\n";
    return l;
  }

  physics::JointPtr joint( std::string name, std::string& errors ) {
    physics::JointPtr j = joint( name );
    if( !j ) errors += "Unable to find joint: " + name + "\n";
    return j;
  }

public:
  model_c( physics::WorldPtr world ) : _world( world ) { }
  model_c( physics::ModelPtr model ) : _model( model ) { _world = _model->GetWorld(); }
  virtual ~model_c( void ) { }

  physics::Link_V links( void ) { return _model->GetLinks(); }
  physics::Joint_V joints( void ) { return _model->GetJoints(); }
  physics::ModelPtr model( void ) { return _model; }
  std::string name( void ) { return _model->GetName(); }

};

//-----------------------------------------------------------------------------
class weazelball_c : public model_c
{
private:
  
  physics::LinkPtr _shell; 
  physics::LinkPtr _axel;
  physics::LinkPtr _motor;
  physics::LinkPtr _gasket;
  
  physics::JointPtr _actuator;

public:

  physics::LinkPtr shell( void ) { return _shell; }
  physics::LinkPtr axel( void ) { return _axel; }
  physics::LinkPtr motor( void ) { return _motor; }
  physics::LinkPtr gasket( void ) { return _gasket; }
  
  physics::JointPtr actuator( void ) { return _actuator; }

public:
  weazelball_c( physics::WorldPtr world ) : model_c( world ) { }
  weazelball_c( physics::ModelPtr model ) : model_c( model ) { }
  virtual ~weazelball_c( void ) { }

  bool validate( std::string& errors ) {
    std::string id;

    if( _model ) {
      _world = _model->GetWorld();
    } else if( _world ) {
      _model = model_c::model( "weazelball", errors );
      if( !_model ) return false;
    } else {
      errors += "No world or model provided to weazelball_c instance\n";
      return false;
    }

    // ur10 arm
    _shell = link( "shell", errors );
    _axel = link( "axel", errors );
    _motor = link( "motor", errors );
    _gasket = link( "gasket", errors );
    _actuator = joint( "actuator", errors );

    if( !( _world && _model && _shell && _axel && _motor && _gasket && _actuator ) ) 
      return false;
    return true;
  }
};

//-----------------------------------------------------------------------------
class world_c 
{
private:
  physics::WorldPtr _world; 

  weazelball_ptr _weazelball;

#ifdef DATA_GENERATION
  boost::shared_ptr<Reveal::DB::database_c> db;

  Reveal::Core::scenario_ptr scenario;
  Reveal::Core::trial_ptr trial;
  Reveal::Core::solution_ptr solution;
#endif

  unsigned trial_index;

public:
  world_c( physics::WorldPtr world ) : _world( world ) { }
  virtual ~world_c( void ) { }

  physics::WorldPtr gzworld( void ) { return _world; }
  weazelball_ptr weazelball( void ) { return _weazelball; }

  bool validate( std::string& errors ) {
    std::string validation_errors;
    bool weazelball_validated;
    errors = "";

    // create the weazelball model encapsulation structure and validate it 
    _weazelball = weazelball_ptr( new weazelball_c( _world ) );

    weazelball_validated = _weazelball->validate( validation_errors );

    if( !( weazelball_validated ) ) return false;
    return true;
  }

  void reset( void ) {
    _world->Reset();
  }

  double sim_time( void ) {
    return _world->GetSimTime().Double();
  }
 
  void sim_time( double t ) {
    _world->SetSimTime( common::Time( t ) );
  }

  double step_size( void ) {
    return _world->GetPhysicsEngine()->GetMaxStepSize();
  }

  double real_time( void ) {
    return _world->GetRealTime().Double();
  }

  bool open( void ) {
#ifdef DATA_GENERATION
    db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c( "localhost" ) );
    db->open();

    trial_index = 0;

    // create and insert the scenario record
    scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

    scenario->id = "weazelball";
    scenario->description = "weazelball robot";
    scenario->trials = 100;  // NOTE: we don't know this in advance and we don't know when it will exit at this point!.
   // number of trials is arbitrary at this point
    scenario->steps_per_trial = 1;

    db->insert( scenario );

    // create and insert the analyzer record
    Reveal::Core::analyzer_ptr analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );
   
    analyzer->scenario_id = scenario->id;
    analyzer->filename = ANALYZER_PATH;
    analyzer->type = Reveal::Core::analyzer_c::PLUGIN;
    db->insert( analyzer );
#endif

    reset();
  }

  void close( void ) {
#ifdef DATA_GENERATION
      db->close();
#endif
  }

// Note: that this define is used just to limit knowledge of the reveal core
// from the controller.  It is entirely arbitrary in this case and designed
// solely to firewall dependencies.
#ifdef REVEAL_SERVICE
  void apply_trial( Reveal::Core::trial_ptr trial ) {

    _world->SetSimTime( trial->t );
    _world->GetPhysicsEngine()->SetMaxStepSize( trial->dt );

    for( unsigned i = 0; i < trial->models.size(); i++ ) {
      Reveal::Core::model_ptr model = trial->models[i];

      physics::ModelPtr gzmodel = _world->GetModel( model->id );
      assert( gzmodel );
      
      for( unsigned j = 0; j < model->links.size(); j++ ) {
        Reveal::Core::link_ptr link = model->links[j];
    
        physics::LinkPtr gzlink = gzmodel->GetLink( link->id );
        assert( gzlink );

        math::Vector3 pos( link->state[0], link->state[1], link->state[2] );
        math::Quaternion rot( link->state[3], link->state[4], link->state[5], link->state[6] );
        math::Vector3 linv( link->state[7], link->state[8], link->state[9] );
        math::Vector3 angv( link->state[10], link->state[11], link->state[12] );

        math::Pose pose( pos, rot );
        gzlink->SetWorldPose( pose );
        gzlink->SetLinearVel( linv );
        gzlink->SetAngularVel( angv );
      }
      for( unsigned j = 0; j < model->joints.size(); j++ ) {
        Reveal::Core::joint_ptr joint = model->joints[j];

        physics::JointPtr gzjoint = gzmodel->GetJoint( joint->id );
        assert( gzjoint );

        // NOTE : GetAngleCount is not implemented for Bullet or ODE
        //unsigned max_k = gzjoint->GetAngleCount();
        //for( unsigned k = 0; k < max_k; k++ )
        //  gzjoint->SetForce( k, joint->control[k] );

        // Bullet and ODE only use 1 DOF?
        gzjoint->SetForce( 0, joint->control[0] ); 
      }
    } 
  }
#endif // REVEAL_SERVICE

  void write_trial( double f ) {
#ifdef DATA_GENERATION
    // record state of the system into initial trial
    trial = Reveal::Core::trial_ptr( new Reveal::Core::trial_c() );

    trial->scenario_id = scenario->id;
    trial->trial_id = trial_index++;
    trial->t = sim_time();
    trial->dt = step_size();

    // arm data
    {
      Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
      model->id = _weazelball->name();

      physics::Link_V gzlinks = _weazelball->links();
      for( unsigned i = 0; i < gzlinks.size(); i++ ) {
        Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
        link->id = gzlinks[i]->GetName();

        //math::Pose pose = gzlinks[i]->GetWorldCoGPose();
        math::Pose pose = gzlinks[i]->GetWorldPose();
        math::Vector3 linv = gzlinks[i]->GetWorldLinearVel();
        math::Vector3 angv = gzlinks[i]->GetWorldAngularVel();

        for( unsigned j = 0; j < 3; j++ )
          link->state[j] = pose.pos[j];
        link->state[3] = pose.rot.w;
        link->state[4] = pose.rot.x;
        link->state[5] = pose.rot.y;
        link->state[6] = pose.rot.z;
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+7] = linv[j];
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+10] = angv[j];
        model->links.push_back( link );
      }

      physics::Joint_V gzjoints = _weazelball->joints();
      for( unsigned i = 0; i < gzjoints.size(); i++ ) {
        Reveal::Core::joint_ptr joint = Reveal::Core::joint_ptr( new Reveal::Core::joint_c() );
        joint->id = gzjoints[i]->GetName();

        if( joint->id == _weazelball->actuator()->GetName() )
          joint->control[0] = f;
        else
          continue;    // bad joint.  Should be unreachable

        model->joints.push_back( joint );
      }

      trial->models.push_back( model );
    }

    db->insert( trial );
    scenario->trials = trial_index;

#endif // DATA_GENERATION
  }

#ifdef DATA_GENERATION
  void write_solution( void ) {
    double t = sim_time();
    double dt = step_size();

    // build a model solution
    solution = Reveal::Core::solution_ptr( new Reveal::Core::solution_c( Reveal::Core::solution_c::MODEL ) );

    solution->scenario_id = scenario->id;
    solution->trial_id = trial->trial_id;
    solution->t = t;
    solution->dt = t;

    {
      Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
      model->id = _weazelball->name();

      physics::Link_V gzlinks = _weazelball->links();
      for( unsigned i = 0; i < gzlinks.size(); i++ ) {
        Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
        link->id = gzlinks[i]->GetName();

        math::Pose pose = gzlinks[i]->GetWorldCoGPose();
        math::Vector3 linv = gzlinks[i]->GetWorldLinearVel();
        math::Vector3 angv = gzlinks[i]->GetWorldAngularVel();

        for( unsigned j = 0; j < 3; j++ )
          link->state[j] = pose.pos[j];
        link->state[3] = pose.rot.w;
        link->state[4] = pose.rot.x;
        link->state[5] = pose.rot.y;
        link->state[6] = pose.rot.z;
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+7] = linv[j];
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+10] = angv[j];
        model->links.push_back( link );
      }
      solution->models.push_back( model );
    }

    db->insert( solution );
  }
#endif // DATA_GENERATION

#ifdef REVEAL_SERVICE
  void extract_solution( Reveal::Core::solution_ptr& solution ) {
    solution->dt = step_size();

    {
      Reveal::Core::model_ptr model = Reveal::Core::model_ptr( new Reveal::Core::model_c() );
      model->id = _weazelball->name();

      physics::Link_V gzlinks = _weazelball->links();
      for( unsigned i = 0; i < gzlinks.size(); i++ ) {
        Reveal::Core::link_ptr link = Reveal::Core::link_ptr( new Reveal::Core::link_c() );
        link->id = gzlinks[i]->GetName();

        math::Pose pose = gzlinks[i]->GetWorldCoGPose();
        math::Vector3 linv = gzlinks[i]->GetWorldLinearVel();
        math::Vector3 angv = gzlinks[i]->GetWorldAngularVel();

        for( unsigned j = 0; j < 3; j++ )
          link->state[j] = pose.pos[j];
        link->state[3] = pose.rot.w;
        link->state[4] = pose.rot.x;
        link->state[5] = pose.rot.y;
        link->state[6] = pose.rot.z;
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+7] = linv[j];
        for( unsigned j = 0; j < 3; j++ )
          link->state[j+10] = angv[j];
        model->links.push_back( link );
      }

      solution->models.push_back( model );
    }

  }
#endif // REVEAL_SERVICE

};

//-----------------------------------------------------------------------------
#endif // _REVEAL_SCENARIO_WEAZELBALL_MODELS_H_
