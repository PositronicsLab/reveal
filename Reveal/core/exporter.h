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

  bool write( scenario_ptr scenario, analyzer_ptr analyzer, solution_ptr ex_solution, trial_ptr ex_trial, std::string delimiter = " " );
  bool write( analyzer_ptr analyzer );

  bool write( double t, double dt, trial_ptr trial );
  bool write( double t, double dt, solution_ptr solution );

private:
  datamap_ptr _trial_column_map;
  datamap_ptr _solution_column_map;

  datawriter_c _trial_datawriter;
  datawriter_c _solution_datawriter;

  std::string _scenario_file;
  std::string _trial_file;
  std::string _solution_file;

  bool write_scenario_element( xml_element_ptr parent, scenario_ptr scenario, analyzer_ptr analyzer, trial_ptr trial, solution_ptr solution, std::string delimiter );
  bool write_analyzer_element( xml_element_ptr parent, analyzer_ptr analyzer, bool reference_only = true );
  bool write_solution_element( xml_element_ptr parent, solution_ptr ex_solution, std::string scenario_id, std::string delimiter );
  bool write_trial_element( xml_element_ptr parent, trial_ptr ex_trial, std::string scenario_id, std::string delimiter );
  bool write_link_element( xml_element_ptr parent, link_ptr link, unsigned& column );
  bool write_joint_element( xml_element_ptr parent, joint_ptr joint, unsigned& column );
  bool add_column_attribute( xml_element_ptr element, unsigned& column, unsigned size = 1 );
  bool add_map_attribute( xml_element_ptr element, std::string map );
  bool add_field_element( xml_element_ptr parent, std::string name, unsigned& column, unsigned size = 1, std::string map = "" );

};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_EXPORTER_H_
