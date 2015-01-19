#ifndef _WEAZELBALL_H_
#define _WEAZELBALL_H_

//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <sstream>

#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------

class wbstate_c {
public:
  wbstate_c( void ) {
    _t = 0.0;
    for( unsigned i = 0; i < size(); i++ ) _x[i] = 0.0;
    _x[3] = 1.0;  // normalize w coordinate
  }
  virtual ~wbstate_c( void ) {}

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
    printf( "wbstate{ t[%f] x{", _t );
    for( unsigned i = 0; i < size(); i++ ) {
      if( i > 0 ) printf( ", " );
      printf( "%f", _x[i] );
    }
    printf( "}}" );
  }
};

//-----------------------------------------------------------------------------
typedef boost::shared_ptr<wbstate_c> wbstate_ptr;

//-----------------------------------------------------------------------------

class wbdata_c;
typedef boost::shared_ptr<wbdata_c> wbdata_ptr;

//-----------------------------------------------------------------------------

class wbdata_c {
public:
  std::vector< std::vector<wbstate_ptr> > states;

  wbdata_c( void ) {
    states.resize( 10 );
  }

  virtual ~wbdata_c( void ) {}

  bool load_mocap( void ) {
    std::string path = "/home/j/weasel-experiment-01-14-15/trials/";

    for( unsigned i = 1; i <= 10; i++ ) {
      std::stringstream ssfile;
      ssfile << "trial" << i << ".txt";
      read_vicon( ssfile.str(), states[i] );
    }
  }

  bool read_vicon( std::string filename, std::vector<wbstate_ptr>& states ) {
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

        wbstate_ptr state = wbstate_ptr( new wbstate_c() );

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

#endif // _WEAZELBALL_H_
