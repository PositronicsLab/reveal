#include "Reveal/db/importer.h"

#include "Reveal/core/xml.h"
#include "Reveal/core/system.h"

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
bool importer_c::read( std::string path, std::string file ) {
  Reveal::Core::xml_c xml;
  Reveal::Core::xml_element_ptr root, element;  

  _path = path;
  _file = file;
  std::string full_path = combine_path( _path, _file );

#ifdef LOCAL_DB
  _db = boost::shared_ptr<Reveal::DB::database_c>( new Reveal::DB::database_c() );
  if( !_db->open() ) return false;
#endif

  //xml.read( _file );
  xml.read( full_path );
  root = xml.root();

  bool result = false;
  for( unsigned i = 0; i < root->elements(); i++ ) {
    element = root->element( i );

    if( element->get_name() == "Scenario" ) {
      Reveal::Core::scenario_ptr scenario;
      result = read_scenario_element( element, scenario );
      if( result && scenario ) {
#ifdef LOCAL_DB
        _db->insert( scenario );
#endif
      }
    } else if( element->get_name() == "Analyzer" ) {
      Reveal::Core::analyzer_ptr analyzer;
      result = read_analyzer_element( element, analyzer );
      if( result && analyzer ) {
#ifdef LOCAL_DB
        _db->insert( analyzer );
#endif
      }
    }
  }

#ifdef LOCAL_DB
  _db->close();
#endif

  return result;
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
/*
    } else if( attribute->get_name() == "trials" ) {
      scenario->trials = (unsigned)atoi( attribute->get_value().c_str() );
*/
    } else if( attribute->get_name() == "sample-rate" ) {
      scenario->sample_rate = (double)atof( attribute->get_value().c_str() );
    }
  }

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Description" ) {
      scenario->description = element->get_value();
    } else if( element->get_name() == "URI" ) {
      scenario->uris.push_back( element->get_value() );
    } else if( element->get_name() == "File" ) {
      read_file_element( element, scenario->id );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::read_analyzer_element( Reveal::Core::xml_element_ptr top, Reveal::Core::analyzer_ptr& analyzer ) {

  Reveal::Core::xml_element_ptr element;
  Reveal::Core::xml_attribute_ptr attribute;

  analyzer = Reveal::Core::analyzer_ptr( new Reveal::Core::analyzer_c() );

  for( unsigned i = 0; i < top->attributes(); i++ ) {
    attribute = top->attribute( i );
    if( attribute->get_name() == "id" ) {
      analyzer->scenario_id = attribute->get_value();
    } else if( attribute->get_name() == "type" ) {
      std::string value = attribute->get_value();
      if( value == "plugin" ) {
        analyzer->type = Reveal::Core::analyzer_c::PLUGIN;
      } else if( value == "script" ) {
        analyzer->type = Reveal::Core::analyzer_c::SCRIPT;
      }
    } else if( attribute->get_name() == "file" ) {
      analyzer->filename = attribute->get_value();
      // note that this is a path on the server and not necessarily on this 
      // system, so should not assume that can validate path here.
    }
  }

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Datum" ) {
      for( unsigned j = 0; j < element->attributes(); j++ ) {
        attribute = element->attribute( j );
        if( attribute->get_name() == "key" ) {
          analyzer->keys.push_back( attribute->get_value() );
        } else if( attribute->get_name() == "label" ) {
          analyzer->labels.push_back( attribute->get_value() );
        }
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// A file element shall consist of a 'name' and 'type' attribute.  It may also
// carry other attributes for particular types.  If either 'name' and 'type'
bool importer_c::read_file_element( Reveal::Core::xml_element_ptr top, std::string scenario_id ) {
  Reveal::Core::xml_attribute_ptr attribute;
  std::string name, type;

  // validate 'name' and 'type' attributes

  // find the 'name' attribute
  attribute = top->attribute( "name" );
  if( !attribute ) return false;
  name = attribute->get_value();

  // find the 'type' attribute
  attribute = top->attribute( "type" );
  if( !attribute ) return false;
  type = attribute->get_value();

  bool result = false;

  // based on 'type' call the appropriate handler
  if( type == "trial" ) {
    result = read_trial_file_element( top, scenario_id );
  } else if( type == "model_solution" ) {
    result = read_solution_file_element( top, scenario_id );
  } else if( type == "analyzer" ) {
    result = read_analyzer_file_element( top );
  }

  return result;
}

//-----------------------------------------------------------------------------
bool importer_c::read_trial_file_element( Reveal::Core::xml_element_ptr top, std::string scenario_id ) {
  std::string delimiter, file_name, file_path;  
  Reveal::Core::xml_attribute_ptr attribute;
  Reveal::Core::trial_ptr trial;

  attribute = top->attribute( "delimiter" );
  if( attribute )
    delimiter = attribute->get_value();
  else
    delimiter = " ";

  // sanity check: validation of 'name' should already have occured in caller
  attribute = top->attribute( "name" );
  if( !attribute )    return false;     // no filename specified 
  file_name = attribute->get_value();

  // build the file path
  file_path = combine_path( _path, file_name );

  // read the column map
  _trial_column_map = Reveal::Core::datamap_ptr( new Reveal::Core::datamap_c() );
  _trial_column_map->map_trial_element( top );
  //_trial_column_map->print();

  // open a data reader to read in the trial data from the data file
  Reveal::Core::datareader_c reader( file_path, " ", _trial_column_map );
  bool result = reader.open();

  if( !result ) {
    //printf( "failed to open reader\n" );
    return false;
  }

  // read in the trial data and insert it into the database
  unsigned i = 0;
  while( reader.read( trial, scenario_id, top ) ) {
    //trial->print();

#ifdef LOCAL_DB
    _db->insert( trial );
#endif
    i++;
  }

  printf( "imported %u trial records\n", i );

  reader.close();
  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::read_solution_file_element( Reveal::Core::xml_element_ptr top, std::string scenario_id ) {
  std::string delimiter, file_name, file_path;  
  Reveal::Core::xml_attribute_ptr attribute;
  Reveal::Core::solution_ptr solution;

  attribute = top->attribute( "delimiter" );
  if( attribute )
    delimiter = attribute->get_value();
  else
    delimiter = " ";

  // sanity check: validation of 'name' should already have occured in caller
  attribute = top->attribute( "name" );
  if( !attribute )    return false;     // no filename specified 
  file_name = attribute->get_value();

  // build the file path
  file_path = combine_path( _path, file_name );

  // read the column map
  _solution_column_map = Reveal::Core::datamap_ptr( new Reveal::Core::datamap_c() );
  _solution_column_map->map_solution_element( top );
  //_solution_column_map->print();
 
  // open a data reader to read in the trial data from the data file
  Reveal::Core::datareader_c reader( file_path, " ", _solution_column_map );
  bool result = reader.open();

  if( !result ) {
//    printf( "failed to open reader\n" );
    return false;
  }

  unsigned i = 0;
  // read in the solution data and insert it into the database
  while( reader.read( solution, scenario_id, top ) ) {
    //solution->print();

#ifdef LOCAL_DB
    _db->insert( solution );
#endif
    i++;
  }

  printf( "imported %u model solution records\n", i );

  reader.close();
  return true;
}

//-----------------------------------------------------------------------------
bool importer_c::read_analyzer_file_element( Reveal::Core::xml_element_ptr top ) {
  std::string filename;  
  Reveal::Core::xml_attribute_ptr attribute;

  // sanity check: validation of 'name' should already have occured in caller
  attribute = top->attribute( "name" );
  if( !attribute )    return false;     // no filename specified 
  filename = attribute->get_value();

  bool result = read( _path, filename );

  return result;
}

//-----------------------------------------------------------------------------
} // namespace DB
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
