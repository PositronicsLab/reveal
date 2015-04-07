#include "Reveal/db/importer.h"

#include "Reveal/core/xml.h"


//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace DB {
//-----------------------------------------------------------------------------

importer_c::importer_c( void ) {

}
//-----------------------------------------------------------------------------
importer_c::~importer_c( void ) {

}

//-----------------------------------------------------------------------------
bool importer_c::read( std::string path, std::string scenario_file ) {
  Reveal::Core::scenario_ptr scenario;
  
  Reveal::Core::xml_c xml;
  Reveal::Core::xml_element_ptr root, element;  

  _path = path;
  _scenario_file = scenario_file;
  std::string full_path;

  if( _path[_path.size()-1] != '/' )
    _path += '/';
  full_path = _path + scenario_file;

#ifdef LOCAL_DB
  _db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c() );
  if( !_db->open() ) return false;
#endif

  xml.read( scenario_file );
  root = xml.root();

  for( unsigned i = 0; i < root->elements(); i++ ) {
    element = root->element( i );

    if( element->get_name() == "Scenario" ) {
      read_scenario_element( element, scenario );
    }
  }

#ifdef LOCAL_DB
  _db->close();
#endif

  return true;
}
//-----------------------------------------------------------------------------
bool importer_c::read_scenario_element( Reveal::Core::xml_element_ptr top, Reveal::Core::scenario_ptr& scenario ) {

  Reveal::Core::xml_element_ptr element;
  Reveal::Core::xml_attribute_ptr attribute;

  scenario = Reveal::Core::scenario_ptr( new Reveal::Core::scenario_c() );

  for( unsigned i = 0; i < top->attributes(); i++ ) {
    attribute = top->attribute( i );
    if( attribute->get_name() == "id" ) {
      scenario->id = attribute->get_value();
    } else if( attribute->get_name() == "trials" ) {
      scenario->trials = (unsigned)atoi( attribute->get_value().c_str() );
    } else if( attribute->get_name() == "steps-per-trial" ) {
      scenario->steps_per_trial = (unsigned)atoi( attribute->get_value().c_str() );
    }
  }

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Description" ) {
      scenario->description = element->get_value();
    } else if( element->get_name() == "URI" ) {
      scenario->uris.push_back( element->get_value() );
    } else if( element->get_name() == "Analyzer" ) {
      read_analyzer_element( element, scenario->id );
    } else if( element->get_name() == "File" ) {
      read_file_element( element, scenario->id );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::read_analyzer_element( Reveal::Core::xml_element_ptr top, std::string scenario_id ) {
/*
  Reveal::Core::xml_attribute_ptr attribute;

  // find the 'type' attribute
  attribute = top->attribute( "type" );
  if( !attribute ) return false;

  if( attribute->get_value() == "trial" ) {
  }
*/
  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::read_file_element( Reveal::Core::xml_element_ptr top, std::string scenario_id ) {
  Reveal::Core::xml_attribute_ptr attribute;

  // find the 'type' attribute
  attribute = top->attribute( "type" );
  if( !attribute ) return false;

  if( attribute->get_value() == "trial" ) {
    read_trial_file_element( top, scenario_id );
  } else if( attribute->get_value() == "model_solution" ) {
    read_solution_file_element( top, scenario_id );
  }

  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::read_trial_file_element( Reveal::Core::xml_element_ptr top, std::string scenario_id ) {
  std::string delimiter, filename;  
  Reveal::Core::xml_attribute_ptr attribute;

  attribute = top->attribute( "delimiter" );
  if( attribute )
    delimiter = attribute->get_value();
  else
    delimiter = " ";

  attribute = top->attribute( "name" );
  if( attribute )
    filename = attribute->get_value();
  else
    return false;     // no filename specified for trial data

  _trial_column_map = Reveal::Core::datamap_ptr( new Reveal::Core::datamap_c() );
  _trial_column_map->map_trial_element( top );
  //_trial_column_map->print();

  Reveal::Core::trial_ptr trial;

  std::string trial_file = scenario_id + ".trials";
  std::string full_path = _path + trial_file;

  Reveal::Core::datareader_c reader( full_path, " ", _trial_column_map );
  bool result = reader.open();

//  if( !result )
//    printf( "failed to open reader\n" );

  for( unsigned i = 1; i <= 1000; i++ ) {
    reader.read( trial, scenario_id, i, top );
    //trial->print();

#ifdef LOCAL_DB
    _db->insert( trial );
#endif
  }

  reader.close();
  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::read_solution_file_element( Reveal::Core::xml_element_ptr top, std::string scenario_id ) {
  std::string delimiter, filename;  
  Reveal::Core::xml_attribute_ptr attribute;

  attribute = top->attribute( "delimiter" );
  if( attribute )
    delimiter = attribute->get_value();
  else
    delimiter = " ";

  attribute = top->attribute( "name" );
  if( attribute )
    filename = attribute->get_value();
  else
    return false;     // no filename specified for solution data

  _solution_column_map = Reveal::Core::datamap_ptr( new Reveal::Core::datamap_c() );
  _solution_column_map->map_solution_element( top );
  //_solution_column_map->print();
 
  std::string trial_file = scenario_id + ".solutions";
  std::string full_path = _path + trial_file;

  Reveal::Core::solution_ptr solution;
  Reveal::Core::datareader_c reader( full_path, " ", _solution_column_map );
  bool result = reader.open();

//  if( !result )
//    printf( "failed to open reader\n" );

  for( unsigned i = 1; i <= 10; i++ ) {
    reader.read( solution, scenario_id, i, top );
    //solution->print();

#ifdef LOCAL_DB
    _db->insert( solution );
#endif
  }

  reader.close();
  return true;
}
/*
//-----------------------------------------------------------------------------
bool importer_c::import_trial_file( Reveal::Core::xml_element_ptr top, std::string filename ) {
  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::import_solution_file( Reveal::Core::xml_element_ptr top, std::string filename ) {
  return true;
}
*/
//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
