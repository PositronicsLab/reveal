#ifndef _REVEAL_DB_IMPORTER_H_
#define _REVEAL_DB_IMPORTER_H_

#include "Reveal/core/pointers.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/datamap.h"

#define LOCAL_DB
#ifdef LOCAL_DB
#include "Reveal/db/database.h"
#endif

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class importer_c {
private:
  Reveal::Core::datamap_ptr _trial_column_map;
  Reveal::Core::datamap_ptr _solution_column_map;

public:
  importer_c( void );
  virtual ~importer_c( void );

  bool read( std::string filename );

private:
#ifdef LOCAL_DB
  boost::shared_ptr<Reveal::DB::database_c> _db;
#endif

  bool read_scenario_element( Reveal::Core::xml_element_ptr element, Reveal::Core::scenario_ptr& scenario );
  bool read_analyzer_element( Reveal::Core::xml_element_ptr element, std::string scenario_id );
  bool read_file_element( Reveal::Core::xml_element_ptr element, std::string scenario_id );
  bool read_trial_file_element( Reveal::Core::xml_element_ptr element, std::string scenario_id );
  bool read_solution_file_element( Reveal::Core::xml_element_ptr element, std::string scenario_id );
/*
  bool import_trial_file( Reveal::Core::xml_element_ptr map, std::string filename );
  bool import_solution_file( Reveal::Core::xml_element_ptr map, std::string filename );
*/
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_IMPORTER_H_
