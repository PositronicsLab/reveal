#ifndef _REVEAL_CORE_DATAMAP_H_
#define _REVEAL_CORE_DATAMAP_H_

//-----------------------------------------------------------------------------

#include <string>
#include <map>
#include <fstream>
#include <boost/shared_ptr.hpp>

#include "Reveal/core/scenario.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/xml.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class datamap_c;
typedef boost::shared_ptr<datamap_c> datamap_ptr;
//-----------------------------------------------------------------------------
class datamap_c {
private:
  std::map<std::string,unsigned> _map;
  unsigned _columns;

public:
  datamap_c( void );
  virtual ~datamap_c( void );

  bool map_trial_element( xml_element_ptr element );
  bool map_solution_element( xml_element_ptr element );
  bool map_model_element( xml_element_ptr element, unsigned& column );
  bool map_link_element( xml_element_ptr element, unsigned& column, std::string parent_key );
  bool map_joint_element( xml_element_ptr element, unsigned& column, std::string parent_key );
  bool map_field_element( xml_element_ptr element, unsigned& column, std::string parent_key = "" );

  void print( void );

  unsigned columns( void );
  unsigned column( std::string key );
};

//-----------------------------------------------------------------------------
class datawriter_c {
private:
  std::string _filename;
  datamap_ptr _column_map;
  bool _open;

  std::vector<double> _cells;
  std::ofstream* _outfile;
  std::string _delimiter;

public:
  datawriter_c( void );
  datawriter_c( std::string filename, std::string delimiter, datamap_ptr column_map );
  virtual ~datawriter_c( void );

  bool open( void );
  void close( void );
  bool write( std::string key, double value, unsigned offset = 0 );
  void flush( void );

  unsigned columns( void );
};

//-----------------------------------------------------------------------------
class datareader_c {
private:
  std::string _filename;
  datamap_ptr _column_map;
  bool _open;

  std::vector<double> _cells;
  std::ifstream* _infile;
  std::string _delimiter;

public:
  datareader_c( void );
  datareader_c( std::string filename, std::string delimiter, datamap_ptr column_map );
  virtual ~datareader_c( void );

  bool open( void );
  void close( void );
  bool read( trial_ptr& trial, std::string scenario_id, unsigned index, xml_element_ptr top );
  bool read( solution_ptr& solution, std::string scenario_id, unsigned index, xml_element_ptr top );

private:
  bool buffer_line( void );
  void print_cells( void );

  bool read_model( component_ptr owner, xml_element_ptr top, std::string key = "" );
  bool read_link( model_ptr owner, xml_element_ptr top, std::string key = "" );
  bool read_joint( model_ptr owner, xml_element_ptr top, std::string key = "" );
  bool read_field( component_ptr owner, xml_element_ptr top, std::string key = "" );

};


//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_DATAMAP_H_
