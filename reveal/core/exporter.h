/*-----------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

-----------------------------------------------------------------------------*/
#ifndef _REVEAL_CORE_EXPORTER_H_
#define _REVEAL_CORE_EXPORTER_H_

#include <string>

#include "reveal/core/datamap.h"
#include "reveal/core/scenario.h"
#include "reveal/core/solution.h"
#include "reveal/core/trial.h"
#include "reveal/core/analyzer.h"
#include "reveal/core/xml.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class exporter_c {
public:
  /// Default constructor
  exporter_c( void );
  /// Destructor
  virtual ~exporter_c( void );

  /// Write a scenario definition to an xml file
  /// @param scenario the scenario definition to write
  /// @param analyzer the associated analyzer definition to write
  /// @param ex_solution an example of the model solution data structure
  /// @param ex_trial an example of the trial data structure
  /// @param delimiter the delimiter to insert between fields in flat data
  /// @return true if the write operation was successful OR false if the 
  ///         operation failed for any reason
  bool write( scenario_ptr scenario, analyzer_ptr analyzer, solution_ptr ex_solution, trial_ptr ex_trial, std::string delimiter = " " );

  /// Write a trial sample to the exporter trial flat file
  /// @param t the time of this trial
  /// @param dt the time-step this trial will take *deprecated info*
  /// @param trial the trial data to write
  /// @return true if the write operation was successful OR false if the 
  ///         operation failed for any reason
  bool write( double t, double dt, trial_ptr trial );

  /// Write a solution sample to the exporter solution flat file
  /// @param t the time of this solution
  /// @param dt the time-step that was taken to yield solution *deprecated info*
  /// @param solution the solution data to write
  /// @return true if the write operation was successful OR false if the 
  ///         operation failed for any reason
  bool write( double t, double dt, solution_ptr solution );

private:
  datamap_ptr _trial_column_map;     //< datamap defining trial file columns
  datamap_ptr _solution_column_map;  //< datamap defining solution file columns

  datawriter_c _trial_datawriter;    //< datawriter handling trial data
  datawriter_c _solution_datawriter; //< datawriter handling solution data

  std::string _scenario_file;       //< path to the scenario xml definition file
  std::string _trial_file;          //< path to the trial flat file
  std::string _solution_file;       //< path to the solution flat file
  std::string _analyzer_file;       //< path to the analyzer xml definition file

  /// Write the analyzer xml file containing the analyzer definition
  /// @param analyzer_file the path to the xml file to write
  /// @param analyzer the analyzer definition to write
  /// @return true if the xml file was successfully written OR false if the 
  ///         operation failed for any reason
  bool write( std::string analyzer_file, analyzer_ptr analyzer );

  /// Write the scenario xml element containing the definition of all scenario
  /// components 
  /// @param parent the parent element of this xml defintion
  /// @param scenario the scenario definition to write
  /// @param analyzer the analyzer definition to write
  /// @param trial the trial definition to write
  /// @param solution the solution definition to write
  /// @delimiter the delimiter to use between flat file elements
  /// @return true if the scenario element was successfully written OR false if
  ///         the write failed for any reason
  bool write_scenario_element( xml_element_ptr parent, scenario_ptr scenario, analyzer_ptr analyzer, trial_ptr trial, solution_ptr solution, std::string delimiter );

  /// Write the analyzer file xml element definining the file containing the 
  /// analyzer xml definition
  /// @param parent the parent element of this xml definition
  /// @return true if the analyzer element was successfully written OR false if
  ///         the write failed for any reason
  bool write_analyzer_file_element( xml_element_ptr parent );

  /// Write the analyzer xml element (do not confuse with the file element) 
  /// containing the analyzer xml definition.  This element can be inside the
  /// scenario xml definition or referenced using a File element so that its
  /// definition is placed into a separate file.
  /// @param parent the xml element to assign this defintion to
  /// @param analyzer the analyzer data to write
  /// @return true if the analyzer element was successfully written OR false if
  ///         the write failed for any reason
  bool write_analyzer_element( xml_element_ptr parent, analyzer_ptr analyzer );

  /// Write the solution file xml element which defines the mapping of all the 
  /// fields in a solution flat file
  /// @param parent the xml element to assign this definition to
  /// @param ex_solution the example structure of the solution used to develop 
  ///        the flat file map
  /// @param delimiter the delimiter to insert between the flat file values
  /// @return true if the solution file element was successfully written OR 
  ///         false if the write failed for any reason
  bool write_solution_file_element( xml_element_ptr parent, solution_ptr ex_solution, std::string delimiter );

  /// Write the trial file xml element which defines the mapping of all the 
  /// fields in a trial flat file
  /// @param parent the xml element to assign this definition to
  /// @param ex_trial the example structure of the trial used to develop the
  ///        flat file map
  /// @param delimiter the delimiter to insert between the flat file values
  /// @return true if the trial file element was successfully written OR false
  ///         if the write failed for any reason
  bool write_trial_file_element( xml_element_ptr parent, trial_ptr ex_trial, std::string delimiter );

  /// Write the link xml element which is a component of both trial and solution
  /// file definitions
  /// @param parent the xml element to assign this definition to
  /// @param link the link to map
  /// @param column the current column count
  /// @return true if the element was successfully written OR false if the write
  ///         failed for any reason
  bool write_link_element( xml_element_ptr parent, link_ptr link, unsigned& column );

  /// Write the joint xml element which is a component of trial file definitions
  /// @param parent the xml element to assign this definition to
  /// @param joint the joint to map
  /// @param column the current column count
  /// @return true if the element was successfully written OR false if the write
  ///         failed for any reason
  bool write_joint_element( xml_element_ptr parent, joint_ptr joint, unsigned& column );

  /// Add an xml attribute defining a column in a flat file
  /// @param element the element to add the attribute to
  /// @param column the current column count
  /// @param size the number of fields this element has (used for vectors)
  /// @return true if the attribute was successfully added OR false if the 
  ///         operation failed for any reason
  bool add_column_attribute( xml_element_ptr element, unsigned& column, unsigned size = 1 );

  /// Add an xml element defining a field (a range of columns) in a flat file
  /// @param parent the parent element to add the field to 
  /// @param name the name/key of the field element
  /// @param column the current column count
  /// @param size the number of fields this element has (used for vectors)
  /// @return true if the element was successfully added OR false if the 
  ///         operation failed for any reason
  bool add_field_element( xml_element_ptr parent, std::string name, unsigned& column, unsigned size = 1 );

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_EXPORTER_H_
