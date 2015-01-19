#include <gazebo/gazebo.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include "models.h"
#include "weazelball.h"

#ifdef VISUALIZE_REAL_DATA
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
//#include <Reveal/core/pointers.h>
//#include <Reveal/core/state.h>
#undef DATA_GENERATION   // TODO: remove when debugged
#endif
#include <sstream>

#define PI 3.14159265359

#define MOTOR_HZ 2.2

// will want to parameterize gains
#define MOTOR_CONTROLLER_KD 0.01


class state_c {
public:
  state_c( void ) {
    _t = 0.0;
    for( unsigned i = 0; i < size(); i++ ) _x[i] = 0.0;
    _x[3] = 1.0;  // normalize w coordinate
  }
  virtual ~state_c( void ) {}

private:
  double _x[7];
  double _t;

public:
  unsigned size( void ) const {
    return 7;
  }

  double t( void ) {
    return _t;
  }

  void t( double t_ ) {
    _t = t_;
  }

  void t( int min, int sec, int ms ) {
    _t = (double)min * 60.0 + (double)sec + (double)ms/1000.0;
  }

  double& val( unsigned i ) { 
    assert( i < size() );
    return _x[i];
  }

  const double& val( unsigned i ) const { 
    assert( i < size() );
    return _x[i];
  }

  double& operator[]( unsigned i ) { 
    assert( i < size() );
    return _x[i];
  }

  const double& operator[]( unsigned i ) const { 
    assert( i < size() );
    return _x[i];
  }

  void print( void ) {
    printf( "state{ t[%f] x{", _t );
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _x[i] );
    }
    printf( "}}" );
  }
};

typedef boost::shared_ptr<state_c> state_ptr;

//-----------------------------------------------------------------------------
namespace gazebo 
{
  class controller_c : public ModelPlugin
  {
  private:
    event::ConnectionPtr _updateConnection;

    world_ptr _world;
    weazelball_ptr _weazelball;

#ifdef VISUALIZE_REAL_DATA
    //std::vector<state_ptr> _states;
    wbdata_ptr _wbdata;
#endif
  public:
    //-------------------------------------------------------------------------
    controller_c( void ) { }

    //-------------------------------------------------------------------------
    virtual ~controller_c( void ) {
      event::Events::DisconnectWorldUpdateBegin( _updateConnection );
 
      _world->close();
    }

    //-------------------------------------------------------------------------
#ifdef VISUALIZE_REAL_DATA
/*
    bool load_real_data( void ) {
      std::string data;
      std::string datapath = "/home/j/weasel-experiment-01-14-15/trials/";
      std::string datafile = "trial1.txt";
      std::string filename = datapath + datafile;
      std::ifstream file( filename.c_str() );
      if( !file.is_open() ) return false;

      int min, sec, ms;
      double pos[3];
      double rot[4];

      int line = 0;
      while( std::getline( file, data ) ) {
        int idx = line++ % 4;
        if( idx == 0 ) {
          //timexxx: min:sec:ms [ex: time(min:sec:ms): 30:23:971]
          //prefix is fixed length so will only parse beginning at space
          size_t start = 18, end;
          std::string values = data.substr( start );
          end = values.find( ":" );
          min = atoi( values.substr( 0, end ).c_str() );
          start = end + 1;
          end = values.find( ":", start );
          sec = atoi( values.substr( start, end ).c_str() );
          start = end + 1;
          ms = atoi( values.substr( start ).c_str() );
        } else if( idx == 1 ) {
          //pos: x, y, z [ex: pos: -0.242842, 1.39527, 1.35857]
          //prefix is fixed length so will only parse beginning at first space
          size_t start = 5, end;
          std::string values = data.substr( start );
          end = values.find( "," );
          pos[0] = atof( values.substr( 0, end ).c_str() );
          start = end + 1;
          end = values.find( ",", start );
          pos[1] = atof( values.substr( start, end ).c_str() );
          start = end + 1;
          pos[2] = atof( values.substr( start ).c_str() );
        } else if( idx == 2 ) {
          //quat: x, y, z, w [ex. quat: 0.817646, -0.535734, 0.145617, -0.15245]
          //prefix is fixed length so will only parse beginning at first space
          size_t start = 6, end;
          std::string values = data.substr( start );
          end = values.find( "," );
          rot[0] = atof( values.substr( 0, end ).c_str() );
          start = end + 1;
          end = values.find( ",", start );
          rot[1] = atof( values.substr( start, end ).c_str() );
          start = end + 1;
          end = values.find( ",", start );
          rot[2] = atof( values.substr( start, end ).c_str() );
          start = end + 1;
          rot[3] = atof( values.substr( start ).c_str() );
        } else if( idx == 3 ) {
          // blank line
          // retain the record
   
          //std::cout << "(" << min << ":" << sec << ":" << ms << "):";
          //std::cout << "[" << pos[0] << "," << pos[1] << "," << pos[2] << ",";
          //std::cout << rot[0] << "," << rot[1] << "," << rot[2] << "," << rot[3] << "]" << std::endl;

          state_ptr state = state_ptr( new state_c() );

          state->t( min, sec, ms );
          for( unsigned i = 0; i < 3; i++ )
            state->val(i) = pos[i];
          for( unsigned i = 0; i < 4; i++ )
            state->val(i+3) = rot[i];
          _states.push_back( state );

          //state->print();
          //printf( "\n" );
        }

        //std::cout << data << std::endl;

      }
      file.close();

      return true;
    }
*/
#endif

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called when the simulation is starting up
    virtual void Load( physics::ModelPtr model, sdf::ElementPtr sdf ) {

      std::string validation_errors;
      _world = world_ptr( new world_c( model->GetWorld() ) );
      if( !_world->validate( validation_errors ) ) {
        printf( "Unable to validate world in controller\n%s\nERROR: Plugin failed to load\n", validation_errors.c_str() );
        return;
      }

      // open the world before we begin
      _world->open();

      // get references to objects in the world
      _weazelball = _world->weazelball();
   
      // -- CALLBACKS --
      _updateConnection = event::Events::ConnectWorldUpdateBegin(
        boost::bind( &controller_c::Update, this ) );

#ifdef DATA_GENERATION
      //double desired_angpos;
      // write the initial trial.  State at t = 0 and no controls
      _world->write_trial( 0.0 );
#endif

#ifdef VISUALIZE_REAL_DATA
      //load_real_data();

      _wbdata = wbdata_ptr( new wbdata_c() );
      _wbdata->load_mocap();
#endif
 
      // -- FIN --
      printf( "controller has initialized\n" );
  
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation advances a timestep
    virtual void Update( ) {
#ifdef DATA_GENERATION
/*
      static bool first_trial = true;

      if( first_trial ) 
        first_trial = false;
      else 
        _world->write_solution();
*/
      _world->write_solution();
#endif

#ifdef VISUALIZE_REAL_DATA
      static unsigned calls = 0;
      static unsigned current_state_idx = 0;

      std::vector< wbstate_ptr >* trial = &_wbdata->states[0];

      if( calls == 0 ) {
        if( current_state_idx == trial->size() ) return;

        wbstate_ptr state = trial->at( current_state_idx++ );

        gazebo::math::Vector3 pos( state->val(0), state->val(1), state->val(2) );
        gazebo::math::Quaternion rot( state->val(6), state->val(3), state->val(4), state->val(5) );
        gazebo::math::Pose pose( pos, rot );

        _weazelball->model()->SetLinkWorldPose( pose, _weazelball->shell() );

        _world->sim_time( state->t() );
      } else {
        if( calls == 100 ) {
          calls = 0;
          return;
        }
      }
      calls++;
/*
      static unsigned calls = 0;
      static unsigned current_state_idx = 0;

      if( calls == 0 ) {
        if( current_state_idx == _states.size() ) return;

        state_ptr state = _states[ current_state_idx++ ];

        gazebo::math::Vector3 pos( state->val(0), state->val(1), state->val(2) );
        gazebo::math::Quaternion rot( state->val(6), state->val(3), state->val(4), state->val(5) );
        gazebo::math::Pose pose( pos, rot );

        _weazelball->model()->SetLinkWorldPose( pose, _weazelball->shell() );

        _world->sim_time( state->t() );
      } else {
        if( calls == 100 ) {
          calls = 0;
          return;
        }
      }
      calls++;
*/
#else
      // get the current time
      double t = _world->sim_time();
      double angvel = _weazelball->actuator()->GetVelocity( 0 );
      double desired_angvel = MOTOR_HZ * ( 2.0 * PI );

      // compute the actuator forces
      double f = MOTOR_CONTROLLER_KD * ( desired_angvel - angvel );

      // set the actuator forces for the weazelball
      _weazelball->actuator()->SetForce( 0, f );
#endif

#ifdef DATA_GENERATION
      _world->write_trial( f );
#endif
    }

    //-------------------------------------------------------------------------
    // Gazebo callback.  Called whenever the simulation is reset
    //virtual void Reset( ) { }

  };

  GZ_REGISTER_MODEL_PLUGIN( controller_c )

} // namespace gazebo

//-----------------------------------------------------------------------------

