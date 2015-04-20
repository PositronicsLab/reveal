#ifndef _REVEAL_CORE_EXPORTER_H_
#define _REVEAL_CORE_EXPORTER_H_

#include <string>

#include "Reveal/core/datamap.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/analyzer.h"
#include "Reveal/core/xml.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class exporter_c {
public:
  exporter_c( void );
  virtual ~exporter_c( void );

  /// write a scenario definition to an xml file
  bool write( scenario_ptr scenario, analyzer_ptr analyzer, solution_ptr ex_solution, trial_ptr ex_trial, std::string delimiter = " " );

  /// write a trial sample to the exporter trial flat file
  bool write( double t, double dt, trial_ptr trial );

  /// write a solution sample to the exporter trial flat file
  bool write( double t, double dt, solution_ptr solution );

private:
  datamap_ptr _trial_column_map;
  datamap_ptr _solution_column_map;

  datawriter_c _trial_datawriter;
  datawriter_c _solution_datawriter;

  std::string _scenario_file;
  std::string _trial_file;
  std::string _solution_file;
  std::string _analyzer_file;

  /// write the analyzer xml file containing the analyzer definition
  bool write( std::string analyzer_file, analyzer_ptr analyzer );

  /// write the scenario xml element containing the definition of all scenario
  /// components 
  bool write_scenario_element( xml_element_ptr parent, scenario_ptr scenario, analyzer_ptr analyzer, trial_ptr trial, solution_ptr solution, std::string delimiter );

  /// write the analyzer file xml element definining the file containing the 
  /// analyzer xml definition
  bool write_analyzer_file_element( xml_element_ptr parent );

  /// write the analyzer xml element (do not confuse with the file element) 
  /// containing the analyzer xml definition.  This element can be inside the
  /// scenario xml definition or referenced using a File element so that its
  /// definition is placed into a separate file.
  bool write_analyzer_element( xml_element_ptr parent, analyzer_ptr analyzer );

  /// write the solution file xml element which defines the mapping of all the 
  /// fields in a solution flat file
  bool write_solution_file_element( xml_element_ptr parent, solution_ptr ex_solution, std::string delimiter );

  /// write the trial file xml element which defines the mapping of all the 
  /// fields in a trial flat file
  bool write_trial_file_element( xml_element_ptr parent, trial_ptr ex_trial, std::string delimiter );

  /// write the link xml element which is a component of both trial and solution
  /// file definitions
  bool write_link_element( xml_element_ptr parent, link_ptr link, unsigned& column );

  /// write the joint xml element which is a component of trial file definitions
  bool write_joint_element( xml_element_ptr parent, joint_ptr joint, unsigned& column );

  /// add an xml attribute defining a column in a flat file
  bool add_column_attribute( xml_element_ptr element, unsigned& column, unsigned size = 1 );

  /// add an xml attribute defining a map in a flat file 
  /// (not observed in importer yet)
  bool add_map_attribute( xml_element_ptr element, std::string map );

  /// add an xml element defining a field (a range of columns) in a flat file
  bool add_field_element( xml_element_ptr parent, std::string name, unsigned& column, unsigned size = 1 );

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_EXPORTER_H_
