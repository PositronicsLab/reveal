#ifndef _REVEAL_DB_IMPORTER_H_
#define _REVEAL_DB_IMPORTER_H_

#include "reveal/core/pointers.h"
#include "reveal/core/scenario.h"
#include "reveal/core/solution.h"
#include "reveal/core/trial.h"
#include "reveal/core/datamap.h"

#define LOCAL_DB
#ifdef LOCAL_DB
#include "reveal/db/database.h"
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

  std::string _path;
  std::string _file;

public:
  importer_c( void );
  virtual ~importer_c( void );

  bool read( std::string path, std::string file );

private:
#ifdef LOCAL_DB
  boost::shared_ptr<Reveal::DB::database_c> _db;
#endif

  bool read_scenario_element( Reveal::Core::xml_element_ptr element, Reveal::Core::scenario_ptr& scenario );
  bool read_analyzer_element( Reveal::Core::xml_element_ptr element, Reveal::Core::analyzer_ptr& analyzer );
  bool read_file_element( Reveal::Core::xml_element_ptr element, std::string scenario_id );
  bool read_trial_file_element( Reveal::Core::xml_element_ptr element, std::string scenario_id );
  bool read_solution_file_element( Reveal::Core::xml_element_ptr element, std::string scenario_id );
  bool read_analyzer_file_element( Reveal::Core::xml_element_ptr element );
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_IMPORTER_H_
