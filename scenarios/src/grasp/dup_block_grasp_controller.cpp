//-----------------------------------------------------------------------------

#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

//-----------------------------------------------------------------------------

class robot_c : public gazebo::ModelPlugin {
private:
  gazebo::event::ConnectionPtr _update_connection;

  gazebo::physics::ModelPtr _model;
  gazebo::physics::WorldPtr _world;

  gazebo::physics::LinkPtr _gripper_l;
  gazebo::physics::LinkPtr _gripper_r;

  gazebo::physics::JointPtr _actuator_l;
  gazebo::physics::JointPtr _actuator_r;

public:
  robot_c( void ) : gazebo::ModelPlugin() { }
  virtual ~robot_c( void ) {
    gazebo::event::Events::DisconnectWorldUpdateBegin( _update_connection );
  }

  void Load( gazebo::physics::ModelPtr model, sdf::ElementPtr sdf ) {
    std::cout << "Loading Plugin" << std::endl;

    _model = model;
    _world = model->GetWorld();
 
    _gripper_l = model->GetLink("gripper_l");
    if( !_gripper_l ) {
      gzerr << "Unable to find link: gripper_l\nPlugin failed to load\n";
      return;
    }

    _gripper_r = model->GetLink("gripper_r");
    if( !_gripper_r ) {
      gzerr << "Unable to find link: gripper_r\nPlugin failed to load\n";
      return;
    }

    _actuator_l = model->GetJoint("actuator_l");
    if( !_actuator_l ) {
      gzerr << "Unable to find joint: actuator_l\nPlugin failed to load\n";
      return;
    } 

    _actuator_r = model->GetJoint("actuator_r");
    if( !_actuator_r ) {
      gzerr << "Unable to find joint: actuator_r\nPlugin failed to load\n";
      return;
    } 

    _update_connection = gazebo::event::Events::ConnectWorldUpdateBegin(
      boost::bind( &robot_c::Update, this ) );

    std::cout << "Plugin Loaded" << std::endl;
  }
  
  void Update( void ) {
    //_actuator_l->SetForce(0, 0.05);
    //_actuator_r->SetForce(0,-0.05);
    _actuator_l->SetForce(0, 5);
    _actuator_r->SetForce(0,-5);
  }
};

//-----------------------------------------------------------------------------

GZ_REGISTER_MODEL_PLUGIN( robot_c )

//-----------------------------------------------------------------------------
