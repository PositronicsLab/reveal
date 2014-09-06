/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

worker.h defines the worker_c thread that handles execution of an analyzer on
a given solution_set submitted by a client

Note: analyze is currently directly called by a client worker rather than run
as a separate thread.
------------------------------------------------------------------------------*/

#ifndef _REVEAL_ANALYTICS_WORKER_H_
#define _REVEAL_ANALYTICS_WORKER_H_

//-----------------------------------------------------------------------------

#include <Reveal/connection.h>
#include <Reveal/database.h>

#include <Reveal/analytics/types.h>
#include <Reveal/analytics/module.h>
#include <Reveal/analysis.h>
#include <Reveal/pointers.h>
#include <Reveal/solution_set.h>

//-----------------------------------------------------------------------------

namespace Reveal {

//-----------------------------------------------------------------------------

namespace Analytics {

//-----------------------------------------------------------------------------

class worker_c {
public:
  Reveal::Core::connection_c _xmitter;
  Reveal::Core::connection_c _receiver;

private:
  boost::shared_ptr<Reveal::DB::database_c> _db;
  int                     _scenario_id; // likely temporary
  int                     _session_id;  // likely temporary

  Reveal::Core::analyzer_ptr                        _analyzer;
  boost::shared_ptr<Reveal::Analytics::module_c>    _module;
  Reveal::Core::analysis_ptr                        _analysis;
  Reveal::Core::solution_set_ptr                    _solution_set;

public:
  status_e status;

public:

  worker_c( void );
  worker_c( boost::shared_ptr<Reveal::DB::database_c> db, int scenario_id, int session_id );
  virtual ~worker_c( void );

  error_e init( void );
  error_e cycle( void );
  error_e shutdown( void );

  error_e read( void );  // ( parameter is protocol Message read from socket)
  error_e query( void );
  error_e load( void );
  error_e analyze( void );
  error_e insert( void );
  error_e idle( void );

  error_e send( void );  // ( parameter is protocol Message to send to socket )
  error_e receive( void );  // ( parameter is protocol Message from socket )
};

//-----------------------------------------------------------------------------

} // namespace Analytics

//-----------------------------------------------------------------------------

} // namespace Reveal

//-----------------------------------------------------------------------------

#endif // _REVEAL_ANALYTICS_WORKER_H_
