#ifndef _WEAZELBALL_H_
#define _WEAZELBALL_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <sstream>

#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
//#define WEAZELBALL_MOTOR_HZ 2.2182    // 133.092 bpm
#define WEAZELBALL_MOTOR_HZ 2.2167    // 

//-----------------------------------------------------------------------------
#define WEAZELBALL_VICON_SESSIONS 10

//-----------------------------------------------------------------------------
class wb_vicon_state_c {
public:
  wb_vicon_state_c( void ) {
    _t = 0.0;
    for( unsigned i = 0; i < size(); i++ ) _x[i] = 0.0;
    _x[3] = 1.0;  // normalize w coordinate
  }
  virtual ~wb_vicon_state_c( void ) {}

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
    printf( "wb_vicon_state{ t[%f] x{", _t );
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _x[i] );
    }
    printf( "}}" );
  }
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr<wb_vicon_state_c> wb_vicon_state_ptr;

//-----------------------------------------------------------------------------

class wb_vicon_data_c;
typedef boost::shared_ptr<wb_vicon_data_c> wb_vicon_data_ptr;

//-----------------------------------------------------------------------------

class wb_vicon_data_c {
public:
  std::vector< std::vector<wb_vicon_state_ptr> > states;

  wb_vicon_data_c( void ) {
    states.resize( WEAZELBALL_VICON_SESSIONS );
  }

  virtual ~wb_vicon_data_c( void ) {}

  bool load_mocap( void ) {
    //std::string path = "/home/j/weasel-experiment-01-14-15/trials/";
    std::string path = "/home/james/weazel-experiment/trials/";

    for( unsigned i = 0; i < WEAZELBALL_VICON_SESSIONS; i++ ) {
      std::stringstream ssfile;
      unsigned fileid = i + 1;
      ssfile << path << "trial" << fileid << ".txt";
      printf( "reading file: %s\n", ssfile.str().c_str() );
      if( !read_vicon( ssfile.str(), states[i] ) )
        printf( "failed to read file: %s\n", ssfile.str().c_str() );
      printf( "states loaded: %u\n", states[i].size() );
    }
  }

  bool read_vicon( std::string filename, std::vector<wb_vicon_state_ptr>& states ) {
    std::string data;
    //std::string datapath = "/home/j/weasel-experiment-01-14-15/trials/";
    //std::string datafile = "trial1.txt";
    //std::string filename = datapath + datafile;
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

        wb_vicon_state_ptr state = wb_vicon_state_ptr( new wb_vicon_state_c() );

        state->t( min, sec, ms );
        for( unsigned i = 0; i < 3; i++ )
          state->val(i) = pos[i];
        for( unsigned i = 0; i < 4; i++ )
          state->val(i+3) = rot[i];
        states.push_back( state );

        //state->print();
        //printf( "\n" );
      }

      //std::cout << data << std::endl;

    }
    file.close();

    return true;
  }
};

//-----------------------------------------------------------------------------
class wb_fused_state_c {
public:
  wb_fused_state_c( void ) {
    _t = 0.0;
    for( unsigned i = 0; i < size(); i++ ) _x[i] = 0.0;
    _x[3] = 1.0;  // normalize w coordinate
  }
  virtual ~wb_fused_state_c( void ) {}

private:
  double _x[8];
  double _t;

public:
  unsigned size( void ) const {
    return 8;
  }

  double t( void ) {
    return _t;
  }

  void t( double t_ ) {
    _t = t_;
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
    printf( "wb_fused_state{ t[%f] x{", _t );
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _x[i] );
    }
    printf( "}}" );
  }
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr<wb_fused_state_c> wb_fused_state_ptr;

//-----------------------------------------------------------------------------

class wb_fused_data_c;
typedef boost::shared_ptr<wb_fused_data_c> wb_fused_data_ptr;

//-----------------------------------------------------------------------------
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
class wb_fused_data_c {
public:
  std::vector< std::vector<wb_fused_state_ptr> > states;

  wb_fused_data_c( void ) {
    states.resize( WEAZELBALL_VICON_SESSIONS );
  }

  virtual ~wb_fused_data_c( void ) {}

  bool load( void ) {
    std::string path = "/home/james/weazel-experiment/data/";

    for( unsigned i = 0; i < WEAZELBALL_VICON_SESSIONS; i++ ) {
      std::stringstream ssfile;
      unsigned fileid = i + 1;
      ssfile << path << "trial_" << std::setfill('0') << std::setw(2) << fileid << ".log";
      printf( "reading file: %s\n", ssfile.str().c_str() );
      if( !read_log( ssfile.str(), states[i] ) )
        printf( "failed to read file: %s\n", ssfile.str().c_str() );
      printf( "states loaded: %u\n", states[i].size() );
    }
  }

  bool read_log( std::string filename, std::vector<wb_fused_state_ptr>& states ) {
    std::string data;
    std::ifstream file( filename.c_str() );
    if( !file.is_open() ) return false;

    double t;
    double pos[3];
    double rot[4];
    double angle;

    int line = 0;
    while( std::getline( file, data ) ) {
      unsigned i = 0;
      boost::char_separator<char> delim(" ");
      boost::tokenizer< boost::char_separator<char> > tokenizer( data, delim );
      //for( boost::tokenizer< char_separator<char> >::iterator it = tokenizer.begin(); it != tokenizer.end(); ++it ) {
      BOOST_FOREACH( const std::string& token, tokenizer ) {
        //std::string token = *it;
        //printf( "%s ", token.c_str() );
        switch( i++ ) {
        default:
        case 0:
          t = atof( token.c_str() );
          break;
        case 1:
          pos[0] = atof( token.c_str() );
          break;
        case 2:
          pos[1] = atof( token.c_str() );
          break;
        case 3:
          pos[2] = atof( token.c_str() );
          break;
        case 4:
          rot[0] = atof( token.c_str() );
          break;
        case 5:
          rot[1] = atof( token.c_str() );
          break;
        case 6:
          rot[2] = atof( token.c_str() );
          break;
        case 7:
          rot[3] = atof( token.c_str() );
          break;
        case 8:
          angle = atof( token.c_str() );
          break;
        }
      }
      //printf( "\n" );

      wb_fused_state_ptr state = wb_fused_state_ptr( new wb_fused_state_c() );

      state->t( t );
      for( unsigned i = 0; i < 3; i++ )
        state->val(i) = pos[i];
      for( unsigned i = 0; i < 4; i++ )
        state->val(i+3) = rot[i];
      state->val(7) = angle;

      states.push_back( state );

      line++;
    }
    file.close();

    return true;
  }
};

//-----------------------------------------------------------------------------
class wb_full_state_c {
public:
  wb_full_state_c( void ) {
    _t = 0.0;
    for( unsigned i = 0; i < size(); i++ ) _x[i] = 0.0;
    _x[3] = 1.0;  // normalize w coordinate
    _x[16] = 1.0;  // normalize w coordinate
  }
  virtual ~wb_full_state_c( void ) {}

private:
  double _x[26];
  double _t;

public:
  unsigned size( void ) const {
    return 26;
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
    printf( "wb_full_state{ t[%f] x{", _t );
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _x[i] );
    }
    printf( "}}" );
  }
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr<wb_full_state_c> wb_full_state_ptr;

//-----------------------------------------------------------------------------

class wb_full_data_c;
typedef boost::shared_ptr<wb_full_data_c> wb_full_data_ptr;

//-----------------------------------------------------------------------------

class wb_full_data_c {
public:
  std::vector< std::vector<wb_full_state_ptr> > states;

  wb_full_data_c( void ) {
    states.resize( WEAZELBALL_VICON_SESSIONS );
  }

  virtual ~wb_full_data_c( void ) {}
/*
  bool load_mocap( void ) {
    //std::string path = "/home/j/weasel-experiment-01-14-15/trials/";
    std::string path = "/home/james/weazel-experiment/trials/";

    for( unsigned i = 0; i <= 9; i++ ) {
    //for( unsigned i = 1; i <= 9; i++ ) {
      std::stringstream ssfile;
      unsigned fileid = i + 1;
      ssfile << path << "trial" << fileid << ".txt";
      printf( "reading file: %s\n", ssfile.str().c_str() );
      if( !read_vicon( ssfile.str(), states[i] ) )
        printf( "failed to read file: %s\n", ssfile.str().c_str() );
      printf( "states loaded: %u\n", states[i].size() );
    }
  }

  bool read_vicon( std::string filename, std::vector<wb_fused_state_ptr>& states ) {
    std::string data;
    //std::string datapath = "/home/j/weasel-experiment-01-14-15/trials/";
    //std::string datafile = "trial1.txt";
    //std::string filename = datapath + datafile;
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

        wb_fused_state_ptr state = wb_fused_state_ptr( new wb_fused_state_c() );

        state->t( min, sec, ms );
        for( unsigned i = 0; i < 3; i++ )
          state->val(i) = pos[i];
        for( unsigned i = 0; i < 4; i++ )
          state->val(i+3) = rot[i];
        states.push_back( state );

        //state->print();
        //printf( "\n" );
      }

      //std::cout << data << std::endl;

    }
    file.close();

    return true;
  }
*/
};


#endif // _WEAZELBALL_H_
