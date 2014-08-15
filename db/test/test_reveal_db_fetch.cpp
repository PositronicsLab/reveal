#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include <Reveal/database.h>
#include <Reveal/pendulum.h>
#include <Reveal/pointers.h>
#include <Reveal/scenario.h>
#include <Reveal/trial.h>
#include <Reveal/solution.h>

#include <Reveal/query.h>
//#include <Reveal/dataset.h>

//-----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
  Reveal::DB::database_c db( "localhost" );

  db.open();

/*
  std::string scenario_doc = "scenario";
  std::string trial_doc = "trial";
  std::string ex_solution_doc = "expected_solution";
  std::string client_solution_doc = "client_solution";
  std::string analytics_doc = "analytics";
*/

  std::auto_ptr<mongo::DBClientCursor> cursor;

  //std::cout << "scenario count:" << db._connection.count( "test.scenario" ) << std::endl;
  Reveal::DB::query_c query;
  std::string scenario = "pendulum";
  //std::set< std::string > fields;

  //Reveal::DB::scenario_dataset_c dataset;
  Reveal::Core::scenario_ptr scenarioptr;
  db.query( scenarioptr, scenario );
  scenarioptr->print();

  for( unsigned i = 0; i < scenarioptr->trials; i++ ) {
    Reveal::Core::trial_ptr trialptr;
    db.query( trialptr, scenarioptr->name, i );
    trialptr->print();
  }

  for( unsigned i = 0; i < scenarioptr->trials; i++ ) {
    Reveal::Core::solution_ptr solutionptr;
    db.query( solutionptr, Reveal::Core::solution_c::MODEL, scenarioptr->name, i );
    solutionptr->print();
  }

  // scenario
/*
  query.scenario( scenario );
  db.fetch( cursor, scenario_doc, query() );
  dataset( cursor );
  dataset.echo();
*/
/*
  query.scenario( scenario );
  db.fetch( cursor, scenario_doc, query() );
  if( cursor->more() ) {
    cursor->peekFirst().getFieldNames( fields );
  }
*/
/*
  for( std::set<std::string>::iterator it = fields.begin(); it != fields.end(); it++ ) {
    if( it != fields.begin() ) std::cout << ", ";
    std::cout << *it;
  }
  std::cout << std::endl;
*/
/*
  while( cursor->more() ) {
    //std::cout << cursor->next().toString() << std::endl;
    mongo::BSONObj b = cursor->next();
    //std::cout << b.toString() << std::endl;
    mongo::BSONObj e = b.getObjectField( "uris" );
//    mongo::BSONElement e = b.getField( "uri" );

    std::vector<mongo::BSONElement> a;
    e.elems( a );
    for( unsigned i = 0; i < a.size(); i++ ) {
      std::cout << a[i];
    }
    std::cout << std::endl;

    //std::cout << e.couldBeArray() << std::endl;
//    std::vector<mongo::BSONElement> a = e.Array();
    //std::vector<mongo::BSONElement> a = b["uri"].Array();
    //std::cout << a[0].toString() << std::endl;
    std::cout << e.toString() << std::endl;

    //std::cout << b.getField( "uri" ).couldBeArray << std::endl;
  }
*/
/*
  // trial
  query.trial( scenario, 0 );
  db.fetch( cursor, trial_doc, query() );
  if( cursor->more() ) {
    cursor->peekFirst().getFieldNames( fields );
  }
  for( std::set<std::string>::iterator it = fields.begin(); it != fields.end(); it++ ) {
    if( it != fields.begin() ) std::cout << ", ";
    std::cout << *it;
  }
  std::cout << std::endl;
  while( cursor->more() )
    std::cout << cursor->next().toString() << std::endl;

  // solution
  query.expected_solution( scenario, 0 );
  db.fetch( cursor, expected_solution_doc, query() );
  if( cursor->more() ) {
    cursor->peekFirst().getFieldNames( fields );
  }
  for( std::set<std::string>::iterator it = fields.begin(); it != fields.end(); it++ ) {
    if( it != fields.begin() ) std::cout << ", ";
    std::cout << *it;
  }
  std::cout << std::endl;
  while( cursor->more() )
    std::cout << cursor->next().toString() << std::endl;
*/
  return 0;
}
