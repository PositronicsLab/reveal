/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

database.h defines the database_c adapter that encapsulates the mongo database
driver.  All interactions with the mongo database are handled through this
interface
------------------------------------------------------------------------------*/

#ifndef _REVEAL_DB_DATABASE_
#define _REVEAL_DB_DATABASE_

//-----------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Reveal/db/service.h"

#include "Reveal/core/pointers.h"
#include "Reveal/core/analysis.h"
#include "Reveal/core/analyzer.h"
#include "Reveal/core/solution.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

class database_c : public boost::enable_shared_from_this<database_c> {
protected:

  service_ptr                  _service;

public:
  // TODO : add friends to access service ptr

  enum error_e {
    ERROR_NONE,
    ERROR_EMPTYSET
  };

  database_c( void );
  virtual ~database_c( void );

  database_ptr ptr( void );
  service_ptr service( void );

  bool open( void );
  void close( void );

public:
  error_e insert( Reveal::Core::user_ptr user );
  error_e query( Reveal::Core::user_ptr& user, const std::string& user_id );

  error_e insert( Reveal::Core::session_ptr session );
  error_e query( Reveal::Core::session_ptr& session, std::string session_id );

  error_e query( Reveal::Core::digest_ptr& digest );

  error_e insert( Reveal::Core::experiment_ptr experiment );
  error_e query( Reveal::Core::experiment_ptr& experiment, std::string experiment_id );
  error_e update_increment_trial_index( Reveal::Core::experiment_ptr experiment );

  error_e insert( Reveal::Core::scenario_ptr scenario );
  error_e query( Reveal::Core::scenario_ptr& scenario, const std::string& scenario_id );

  error_e insert( Reveal::Core::trial_ptr trial );
  //error_e query( Reveal::Core::trial_ptr& trial, const std::string& scenario_id, unsigned trial_id );
  error_e query( Reveal::Core::trial_ptr& trial, const std::string& scenario_id, double t, double epsilon );

  error_e insert( Reveal::Core::solution_ptr solution );
  error_e query( Reveal::Core::solution_ptr& solution, const std::string& experiment_id, const std::string& scenario_id, double t, double epsilon );
  error_e query( Reveal::Core::solution_ptr& solution, Reveal::Core::solution_c::type_e type, const std::string& scenario_id, double t, double epsilon );

  error_e insert( Reveal::Core::analyzer_ptr analyzer );
  error_e query( Reveal::Core::analyzer_ptr& analyzer, const std::string& scenario_id );
  error_e query( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id );
  error_e query( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id, double t, double epsilon );
  error_e query( Reveal::Core::solution_set_ptr& solution_set, const std::string& experiment_id, double trial_time, double solution_time, double epsilon );
  error_e insert( Reveal::Core::analysis_ptr analysis );
};

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_DB_DATABASE_
