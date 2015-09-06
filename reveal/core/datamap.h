/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

datamap.h defines the classes for mapping data to and from fields in a flat file
using an xml definition and for classes that are capable of reading and writing 
data from flat files into reveal
------------------------------------------------------------------------------*/
#ifndef _REVEAL_CORE_DATAMAP_H_
#define _REVEAL_CORE_DATAMAP_H_

//-----------------------------------------------------------------------------

#include <string>
#include <map>
#include <fstream>
#include <boost/shared_ptr.hpp>

#include "reveal/core/scenario.h"
#include "reveal/core/solution.h"
#include "reveal/core/trial.h"
#include "reveal/core/xml.h"

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
  std::map<std::string,unsigned> _map;     //< the map of keys to indices
  unsigned _columns;                       //< the number of columns in the map

public:
  /// Default constructor
  datamap_c( void );
  /// Destructor
  virtual ~datamap_c( void );

  /// Reads a trial xml element and stores the column map definition
  /// @param element the xml element to read
  /// @return true if mapping was successful OR false if mapping failed for any
  ///         reason 
  bool map_trial_element( xml_element_ptr element );

  /// Reads a solution xml element and stores the column map definition
  /// @param element the xml element to read
  /// @return true if mapping was successful OR false if mapping failed for any
  ///         reason 
  bool map_solution_element( xml_element_ptr element );

  /// Reads a model xml element and stores the column map definition
  /// @param element the xml element to read
  /// @param column the current column count
  /// @return true if mapping was successful OR false if mapping failed for any
  ///         reason 
  bool map_model_element( xml_element_ptr element, unsigned& column );

  /// Reads a link xml element and stores the column map definition
  /// @param element the xml element to read
  /// @param column the current column count
  /// @param parent_key the parent key of the current element
  /// @return true if mapping was successful OR false if mapping failed for any
  ///         reason 
  bool map_link_element( xml_element_ptr element, unsigned& column, std::string parent_key );

  /// Reads a joint xml element and stores the column map definition
  /// @param element the xml element to read
  /// @param column the current column count
  /// @param parent_key the parent key of the current element
  /// @return true if mapping was successful OR false if mapping failed for any
  ///         reason 
  bool map_joint_element( xml_element_ptr element, unsigned& column, std::string parent_key );

  // TODO: update documentation
  /// Reads a field xml element and stores the column map definition
  /// @param element the xml element to read
  /// @param column the current column count
  /// @param parent_key the parent key of the current element
  /// @return true if mapping was successful OR false if mapping failed for any
  ///         reason 
  bool map_field_element( xml_element_ptr element, unsigned& column, std::string parent_key = "" );

  bool map_link_field_element( xml_element_ptr element, unsigned& column, std::string parent_key = "" );
  bool map_joint_field_element( xml_element_ptr element, unsigned& column, std::string parent_key = "" );

  /// Prints the columns map to the console
  void print( void );

  /// Returns the number of columns in the map
  /// @return the number of columns in the map
  unsigned columns( void );

  /// Returns the column index for a field that matches the provided key
  /// @param key the key of the column to index
  /// @return the index of the column named by the corresponding key
  unsigned column( std::string key );
};

//-----------------------------------------------------------------------------
class datawriter_c {
private:
  std::string _filename;     //< the name of the file to write to
  datamap_ptr _column_map;   //< the column map defining the field ordering
  bool _open;                //< flag indicating if the file is open for writing

  std::vector<double> _cells;//< the buffer containing the current data to write
  std::ofstream* _outfile;   //< the output file stream
  std::string _delimiter;    //< the delimiter to write between each cell

public:
  /// Default constructor
  datawriter_c( void );
  /// Parametric constructor
  /// @param filename the path of the file to write to
  /// @param delimiter the delimiter to write between fields
  /// @param column_map the column map to use when mapping values to fields
  datawriter_c( std::string filename, std::string delimiter, datamap_ptr column_map );
  /// Destructor
  virtual ~datawriter_c( void );

  /// Open the output file for writing
  /// @return true if the file is open for writing OR false if the file cannot 
  ///         be opened
  bool open( void );

  /// Close the output file
  void close( void );

  /// Write a value to the buffer
  /// @param key the name of the field to write to
  /// @param value the value to write
  /// @param offset if a vector field, the index of the subfield to write to
  /// @return true if the value was written to the buffer OR false if the value
  ///         could not be written for any reason
  bool write( std::string key, double value, unsigned offset = 0 );

  /// Flushes the buffer into the output stream.  Flush must be called to ensure
  /// that data inserted by write is actually written to the physical disk
  void flush( void );

  /// Returns the number of columns the datawriter has defined
  /// @return the number of columns the datawriter has defined
  unsigned columns( void );
};

//-----------------------------------------------------------------------------
class datareader_c {
private:
  std::string _filename;     //< the name of the file to read from
  datamap_ptr _column_map;   //< the column map defining the field ordering
  bool _open;                //< flag indicating if the file is open for reading

  std::vector<double> _cells;//< the buffer containing the data recently read
  std::ifstream* _infile;    //< the input file stream
  std::string _delimiter;    //< the delimiter separating each value in the file

public:
  /// Default constructor
  datareader_c( void );
  /// Parametric constructor
  /// @param filename the path of the file to read from
  /// @param delimiter the delimiter separating the values in the file
  /// @param column_map the column map to use when mapping fields to values
  datareader_c( std::string filename, std::string delimiter, datamap_ptr column_map );
  /// Destructor
  virtual ~datareader_c( void );

  /// Open the input file for reading
  /// @return true if the file is open for reading OR false if the file cannot 
  ///         be opened
  bool open( void );

  /// Close the output file
  void close( void );

  /// Read an xml trial definition and generate a model of the trial
  /// @param trial a reference to the trial data if reading was successful
  /// @param scenario_id the scenario identifier for the trial data
  /// @param top the parent xml element in the succeeding hierarchy
  /// @return true if the definition was read successfully OR false if 
  /// the read failed for any reason
  bool read( trial_ptr& trial, std::string scenario_id, xml_element_ptr top );

  /// Read an xml solution definition and generate a model of the solution
  /// @param trial a reference to the solution data if reading was successful
  /// @param scenario_id the scenario identifier for the solution data
  /// @param top the parent xml element in the succeeding hierarchy
  /// @return true if the definition was read successfully OR false if the read 
  ///         failed for any reason
  bool read( solution_ptr& solution, std::string scenario_id, xml_element_ptr top );

private:
  /// Buffers the next line in the file into the cells
  /// @return true if the read was successful OR false if the read failed in any
  ///         way
  bool buffer_line( void );

  /// Prints the current contents of the cells to the console
  void print_cells( void );

  /// Read a set of model data
  /// @param owner the reveal component that this part of the hierarchy belongs
  /// @param top the top of the xml element corresponding to this element
  /// @param key any key prefix that may need to be applied to this element
  /// @return true if the definition was read successfully OR false if the read 
  ///         failed for any reason
  bool read_model( component_ptr owner, xml_element_ptr top, std::string key = "" );

  /// Read an xml link definition
  /// @param owner the reveal component that this part of the hierarchy belongs
  /// @param top the top of the xml element corresponding to this element
  /// @param key any key prefix that may need to be applied to this element
  /// @return true if the definition was read successfully OR false if the read 
  ///         failed for any reason
  bool read_link( model_ptr owner, xml_element_ptr top, std::string key = "" );

  /// Read an xml joint definition
  /// @param owner the reveal component that this part of the hierarchy belongs
  /// @param top the top of the xml element corresponding to this element
  /// @param key any key prefix that may need to be applied to this element
  /// @return true if the definition was read successfully OR false if the read 
  ///         failed for any reason
  bool read_joint( model_ptr owner, xml_element_ptr top, std::string key = "" );

  /// Read an xml field definition
  /// @param owner the reveal component that this part of the hierarchy belongs
  /// @param top the top of the xml element corresponding to this element
  /// @param key any key prefix that may need to be applied to this element
  /// @return true if the definition was read successfully OR false if the read 
  ///         failed for any reason
  bool read_field( component_ptr owner, xml_element_ptr top, std::string key = "" );

};


//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_DATAMAP_H_
