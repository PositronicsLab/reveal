#ifndef _REVEAL_CLIENT_IMPORTER_H_
#define _REVEAL_CLIENT_IMPORTER_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "Reveal/db/database.h"
#include "Reveal/core/pointers.h"
#include "Reveal/core/scenario.h"
#include "Reveal/core/trial.h"
#include "Reveal/core/solution.h"
#include "Reveal/core/analyzer.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Client {
//-----------------------------------------------------------------------------

class importer_c {
public:
  importer_c( void );
  virtual ~importer_c( void );

  bool open( void );
  void close( void );

  bool import_scenario( std::string id, std::string description, unsigned trials );
  bool import_analyzer( std::string scenario_id, std::string analyzer_path, Reveal::Core::analyzer_c::type_e type );

  bool import_trial( std::string scenario_id, unsigned trial_id, double t, double dt, std::vector<Reveal::Core::model_ptr> models ); // also requires forces 

  bool import_solution( std::string scenario_id, unsigned trial_id, double t, double dt, std::vector<Reveal::Core::link_ptr> links );
protected:
  boost::shared_ptr<Reveal::DB::database_c> db;

  //Reveal::Core::scenario_ptr scenario;
  //Reveal::Core::trial_ptr trial;
  //Reveal::Core::solution_ptr solution;

};

//-----------------------------------------------------------------------------
} // namespace Client
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CLIENT_IMPORTER_H_
