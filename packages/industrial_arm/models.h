#ifndef _REVEAL_SCENARIO_ARM_MODELS_H_
#define _REVEAL_SCENARIO_ARM_MODELS_H_

#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

using namespace gazebo;

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
};

class arm_c : public model_c
{
private:
  // UR10 arm
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

public:
  physics::LinkPtr base( void ) { return _base; }
  physics::LinkPtr shoulder( void ) { return _shoulder; }
  physics::LinkPtr upperarm( void ) { return _upperarm; }
  physics::LinkPtr forearm( void ) { return _forearm; }
  physics::LinkPtr wrist1( void ) { return _wrist1; }
  physics::LinkPtr wrist2( void ) { return _wrist2; }
  physics::LinkPtr wrist3( void ) { return _wrist3; }
  
  physics::JointPtr shoulder_pan_actuator( void ) { return _shoulder_pan_actuator; }
  physics::JointPtr shoulder_lift_actuator( void ) { return _shoulder_lift_actuator; }
  physics::JointPtr elbow_actuator ( void ) { return _elbow_actuator; }
  physics::JointPtr wrist1_actuator( void ) { return _wrist1_actuator; }
  physics::JointPtr wrist2_actuator( void ) { return _wrist2_actuator; }
  physics::JointPtr wrist3_actuator( void ) { return _wrist3_actuator; }

  // Schunk hand
  physics::LinkPtr hand( void ) { return _hand; }
  physics::LinkPtr finger_l( void ) { return _finger_l; }
  physics::LinkPtr finger_r( void ) { return _finger_r; }

  physics::JointPtr finger_actuator_l( void ) { return _finger_actuator_l; }
  physics::JointPtr finger_actuator_r( void ) { return _finger_actuator_r; } 

public:
  arm_c( physics::ModelPtr model ) : model_c( model ) { }
  virtual ~arm_c( void ) { }

  bool validate( std::string& errors ) {
    std::string id;
    errors = "";

    _world = _model->GetWorld();

    // ur10 arm
    _base = link( "ur10::base_link", errors );
    _shoulder = link( "ur10::shoulder_link", errors );
    _upperarm = link( "ur10::upper_arm_link", errors );
    _forearm = link( "ur10::forearm_link", errors );
    _wrist1 = link( "ur10::wrist_1_link", errors );
    _wrist2 = link( "ur10::wrist_2_link", errors );
    _wrist3 = link( "ur10::wrist_3_link", errors );
    _shoulder_pan_actuator = joint( "ur10::shoulder_pan_joint", errors );
    _shoulder_lift_actuator = joint( "ur10::shoulder_lift_joint", errors );
    _elbow_actuator = joint( "ur10::elbow_joint", errors );
    _wrist1_actuator = joint( "ur10::wrist_1_joint", errors );
    _wrist2_actuator = joint( "ur10::wrist_2_joint", errors );
    _wrist3_actuator = joint( "ur10::wrist_3_joint", errors );

    // schunk hand
    _hand = link( "schunk_mpg_80::base", errors );
    _finger_l = link( "schunk_mpg_80::l_finger", errors );
    _finger_r = link( "schunk_mpg_80::r_finger", errors );
    _finger_actuator_l = joint( "schunk_mpg_80::l_finger_actuator", errors );
    _finger_actuator_r = joint( "schunk_mpg_80::r_finger_actuator", errors );

    if( !( _world && _model && _base && _shoulder && _upperarm && _forearm &&
           _wrist1 && _wrist2 && _wrist3 && _shoulder_pan_actuator && 
           _shoulder_lift_actuator && _elbow_actuator && _wrist1_actuator && 
           _wrist2_actuator && _wrist3_actuator && _hand && _finger_l && 
           _finger_r && _finger_actuator_l && _finger_actuator_r ) )
      return false;
    return true;
  }
};

class target_c : public model_c
{
private:
  physics::LinkPtr _link; 

public:
  physics::LinkPtr link( void ) { return _link; } 
  physics::ModelPtr model( void ) { return _model; } 
  
public:
  target_c( physics::WorldPtr world ) : model_c( world ) { }
  virtual ~target_c( void ) { }

  bool validate( std::string& errors ) {
    std::string id;
    errors = "";

    _model = model_c::model( "block", errors );
    if( _model )
      _link = model_c::link( "body", errors );

    if( !( _world && _model && _link ) )
      return false;
    return true;
  }
};

typedef boost::shared_ptr< model_c > model_ptr;
typedef boost::shared_ptr< arm_c > arm_ptr;
typedef boost::shared_ptr< target_c > target_ptr;

#endif // _REVEAL_SCENARIO_ARM_MODELS_H_
