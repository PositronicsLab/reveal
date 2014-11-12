#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

typedef double Real;

#define PI 3.14159265359

namespace gazebo
{
  class pendulum_controller_c : public ModelPlugin
  {
  private:

    event::ConnectionPtr updateConnection;

    physics::ModelPtr model;
    physics::WorldPtr world;

    physics::Joint_V joints;

    double start_time, last_time;
  private:
    /// The position trajectory function.  A cubic
    Real position_trajectory( Real t, Real tfinal, Real q0, Real qdes ) {

      if( t > tfinal )
        return qdes;
      return -2 * (qdes - q0) / (tfinal*tfinal*tfinal) * (t*t*t) + 3 * (qdes - q0) / (tfinal*tfinal) * (t*t) + q0;
    }

    /// The velocity trajectory function.  The first derivative of the cubic
    Real velocity_trajectory( Real t, Real tfinal, Real q0, Real qdes ) {

      if( t > tfinal )
        return 0.0;
      return -6 * (qdes - q0) / (tfinal*tfinal*tfinal) * (t*t) + 6 * (qdes - q0) / (tfinal*tfinal) * (t);
    }

    /// Control function for Standalone Controller
    Real control( Real time, Real position, Real velocity ) {

      //const Real Kp = 1.0;
      //const Real Kv = 0.1;

      const Real Kp = 100.0;
      const Real Kv = 10.0;

      Real measured_position = position;
      Real measured_velocity = velocity;

      //Real desired_position = position_trajectory( time, 0.5, 0.0, PI );
      //Real desired_velocity = velocity_trajectory( time, 0.5, 0.0, PI );

      Real desired_position = position_trajectory( time, 10.0, 0.0, PI );
      Real desired_velocity = velocity_trajectory( time, 10.0, 0.0, PI );

      Real torque = Kp * ( desired_position - measured_position ) + Kv * ( desired_velocity - measured_velocity );

      return torque;
    }

  public:

    pendulum_controller_c( ) {

    }

    ~pendulum_controller_c( ) {
      event::Events::DisconnectWorldUpdateBegin( this->updateConnection );
    }

  protected:

    void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf) {
      this->model = _model;
      this->world = _model->GetWorld();
      this->joints = this->model->GetJoints();


      std::cerr << "Starting Pendulum Standup Controller Plugin" << std::endl;

      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          boost::bind( &pendulum_controller_c::Update, this ) );

      start_time = world->GetSimTime().Double();
      last_time = start_time;

    }

    void Update( ) {
      double sim_time = world->GetSimTime().Double();
      std::cerr << "Model Plugin Callback: " << sim_time << std::endl;

      Real time = sim_time - start_time;
      Real position = this->joints[0]->GetAngle( 0 ).Radian( );
      Real velocity = this->joints[0]->GetVelocity( 0 );
      Real torque = control( time, position, velocity );

      this->joints[0]->SetForce( 0, torque );

      last_time = sim_time;
    }

  };

  GZ_REGISTER_MODEL_PLUGIN( pendulum_controller_c )

} // namespace gazebo


