#include "Reveal/core/exporter.h"

#include <sstream>

#include "Reveal/core/system.h"

#define DEFINE_COLUMNS
//#define DEFINE_MAP

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

exporter_c::exporter_c( void ) {

}
//-----------------------------------------------------------------------------
exporter_c::~exporter_c( void ) {

}

//-----------------------------------------------------------------------------
bool exporter_c::write( scenario_ptr scenario, analyzer_ptr analyzer, solution_ptr ex_solution, trial_ptr ex_trial, std::string delimiter ) {
  xml_c xml;
  xml_element_ptr root;
  bool result;

  std::stringstream ss_scenario_file, ss_trial_file, ss_solution_file, ss_analyzer_file;

  ss_scenario_file << scenario->id << ".scenario";
  _scenario_file = ss_scenario_file.str();

  ss_trial_file << scenario->id << ".trials";
  _trial_file = ss_trial_file.str();

  ss_solution_file << scenario->id << ".solutions";
  _solution_file = ss_solution_file.str();

  ss_analyzer_file << scenario->id << ".analyzer";
  _analyzer_file = ss_analyzer_file.str();

  // create the xml root for the scenario file
  root = xml_element_ptr( new xml_element_c() );
  root->set_name( "Reveal" );

  // create column maps for the trial and solution data
  _trial_column_map = datamap_ptr( new datamap_c() );
  _solution_column_map = datamap_ptr( new datamap_c() );

  // write the scenario information to the root
  result = write_scenario_element( root, scenario, analyzer, ex_trial, ex_solution, delimiter );
  if( !result ) return result;

  // write the scenario xml file
  xml.root( root );
  xml.write( _scenario_file );

  // write the analyzer file
  result = write( _analyzer_file, analyzer );
  if( !result ) return false;

  //_trial_column_map->print();
  //_solution_column_map->print();

  // open the trial datawriter to append trial data to a flat file
  _trial_datawriter = Reveal::Core::datawriter_c( _trial_file, delimiter, _trial_column_map );
  result = _trial_datawriter.open();
  if( !result ) return false;

  // open the solution datawriter to append solution data to a flat file
  _solution_datawriter = Reveal::Core::datawriter_c( _solution_file, delimiter, _solution_column_map );
  result = _solution_datawriter.open();
  if( !result ) return false;

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write( std::string analyzer_file, analyzer_ptr analyzer ) {
  xml_c xml;
  xml_element_ptr root;

  root = xml_element_ptr( new xml_element_c() );
  root->set_name( "Reveal" );

  write_analyzer_element( root, analyzer );

  xml.root( root );
  xml.write( analyzer_file );

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write( double t, double dt, trial_ptr trial ) {
  _trial_datawriter.write( "time", t );
  _trial_datawriter.write( "time-step", dt );

  for( unsigned i = 0; i < trial->models.size(); i++ ) {
    Reveal::Core::model_ptr model = trial->models[i];
    std::string key;
    for( unsigned j = 0; j < model->links.size(); j++ ) {
      Reveal::Core::link_ptr link = model->links[j];
      key = model->id + "::" + link->id + "::position";
      _trial_datawriter.write( key, link->state.linear_x(), 0 );
      _trial_datawriter.write( key, link->state.linear_y(), 1 );
      _trial_datawriter.write( key, link->state.linear_z(), 2 );
      key = model->id + "::" + link->id + "::rotation";
      _trial_datawriter.write( key, link->state.angular_x(), 0 );
      _trial_datawriter.write( key, link->state.angular_y(), 1 );
      _trial_datawriter.write( key, link->state.angular_z(), 2 );
      _trial_datawriter.write( key, link->state.angular_w(), 3 );
      key = model->id + "::" + link->id + "::linear-velocity";
      _trial_datawriter.write( key, link->state.linear_dx(), 0 );
      _trial_datawriter.write( key, link->state.linear_dy(), 1 );
      _trial_datawriter.write( key, link->state.linear_dz(), 2 );
      key = model->id + "::" + link->id + "::angular-velocity";
      _trial_datawriter.write( key, link->state.angular_dx(), 0 );
      _trial_datawriter.write( key, link->state.angular_dy(), 1 );
      _trial_datawriter.write( key, link->state.angular_dz(), 2 );
    }
    for( unsigned j = 0; j < model->joints.size(); j++ ) {
      Reveal::Core::joint_ptr joint = model->joints[j];
      key = model->id + "::" + joint->id + "::control";
      _trial_datawriter.write( key, joint->control.linear_x(), 0 );
      _trial_datawriter.write( key, joint->control.linear_y(), 1 );
      _trial_datawriter.write( key, joint->control.linear_z(), 2 );
      _trial_datawriter.write( key, joint->control.angular_x(), 3 );
      _trial_datawriter.write( key, joint->control.angular_y(), 4 );
      _trial_datawriter.write( key, joint->control.angular_z(), 5 );
    }
  }

  _trial_datawriter.flush();

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write( double t, double dt, solution_ptr solution ) {
  _solution_datawriter.write( "time", t );
  _solution_datawriter.write( "time-step", dt );
  if( solution->type == Reveal::Core::solution_c::CLIENT )
    _solution_datawriter.write( "real-time", solution->real_time );

  for( unsigned i = 0; i < solution->models.size(); i++ ) {
    Reveal::Core::model_ptr model = solution->models[i];
    std::string key;
    for( unsigned j = 0; j < model->links.size(); j++ ) {
      Reveal::Core::link_ptr link = model->links[j];
      key = model->id + "::" + link->id + "::position";
      _solution_datawriter.write( key, link->state.linear_x(), 0 );
      _solution_datawriter.write( key, link->state.linear_y(), 1 );
      _solution_datawriter.write( key, link->state.linear_z(), 2 );
      key = model->id + "::" + link->id + "::rotation";
      _solution_datawriter.write( key, link->state.angular_x(), 0 );
      _solution_datawriter.write( key, link->state.angular_y(), 1 );
      _solution_datawriter.write( key, link->state.angular_z(), 2 );
      _solution_datawriter.write( key, link->state.angular_w(), 3 );
      key = model->id + "::" + link->id + "::linear-velocity";
      _solution_datawriter.write( key, link->state.linear_dx(), 0 );
      _solution_datawriter.write( key, link->state.linear_dy(), 1 );
      _solution_datawriter.write( key, link->state.linear_dz(), 2 );
      key = model->id + "::" + link->id + "::angular-velocity";
      _solution_datawriter.write( key, link->state.angular_dx(), 0 );
      _solution_datawriter.write( key, link->state.angular_dy(), 1 );
      _solution_datawriter.write( key, link->state.angular_dz(), 2 );
    }
  }

  _solution_datawriter.flush();
 
  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write_scenario_element( xml_element_ptr parent, scenario_ptr scenario, analyzer_ptr analyzer, trial_ptr trial, solution_ptr solution, std::string delimiter ) {
  xml_c xml;
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;

  assert( analyzer );  // warning supression

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "Scenario" );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "id" );
  attribute->set_value( scenario->id );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "package-id" );
  attribute->set_value( scenario->package_id );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "sample-rate" );
  attribute->set_value( scenario->sample_rate );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "sample-start-time" );
  attribute->set_value( scenario->sample_start_time );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "sample-end-time" );
  attribute->set_value( scenario->sample_end_time );
  top->append( attribute );

  element = xml_element_ptr( new xml_element_c() );
  element->set_name( "Description" );
  element->set_value( scenario->description );
  top->append( element );

  for( unsigned i = 0; i < scenario->uris.size(); i++ ) {
    element = xml_element_ptr( new xml_element_c() );
    element->set_name( "URI" );
    element->set_value( scenario->uris[i] );
    top->append( element );
  }

  write_analyzer_file_element( top );
  write_trial_file_element( top, trial, delimiter );
  write_solution_file_element( top, solution, delimiter );

  parent->append( top );

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write_analyzer_element( xml_element_ptr parent, analyzer_ptr analyzer ) {
  xml_c xml;
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;

  // Note: really only should have to specify the filename for scenario 
  // generation.  The rest of the record needs to be exported in a separate 
  // framework as the server needs to validate the pathing and the typing
  // and it requires a compilation step on the server anyway.

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "Analyzer" );

  // specify the scenario
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "id" );
  attribute->set_value( analyzer->scenario_id );
  top->append( attribute );

  // specify the type
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "type" );
  if( analyzer->type == Reveal::Core::analyzer_c::PLUGIN ) 
    attribute->set_value( "plugin" );
  else 
    attribute->set_value( "script" );
  top->append( attribute );

  // specify the anaylzer file path
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "file" );
  attribute->set_value( analyzer->filename );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "source-path" );
  attribute->set_value( analyzer->source_path );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "build-path" );
  attribute->set_value( analyzer->build_path );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "build-target" );
  attribute->set_value( analyzer->build_target );
  top->append( attribute );

  // iterate over any keys and write them as a new element
  for( unsigned i = 0; i < analyzer->keys.size(); i++ ) {
    element = xml_element_ptr( new xml_element_c() );
    element->set_name( "Datum" );

    // write out any keys
    attribute = xml_attribute_ptr( new xml_attribute_c() );
    attribute->set_name( "key" );
    attribute->set_value( analyzer->keys[i] );
    element->append( attribute );

    if( analyzer->keys.size() == analyzer->labels.size() ) {
      // if there are the same number of keys and labels, write out labels too
  
      attribute = xml_attribute_ptr( new xml_attribute_c() );
      attribute->set_name( "label" );
      attribute->set_value( analyzer->labels[i] );
      element->append( attribute );
    }
    top->append( element );
  }
    
  parent->append( top );

  return true;

}
//-----------------------------------------------------------------------------
bool exporter_c::write_analyzer_file_element( xml_element_ptr parent ) {
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;
  //unsigned column = 1;

  // create the file element as the top (root) of this hierarchy
  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "File" );

  // specify the type
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "type" );
  attribute->set_value( "analyzer" );
  top->append( attribute );

  // specify the file name
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "name" );
  attribute->set_value( _analyzer_file );
  top->append( attribute );

  // add this hierarchy to the parent node
  parent->append( top );

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write_solution_file_element( xml_element_ptr parent, solution_ptr ex_solution, std::string delimiter ) {
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;
  unsigned column = 1;

  // create the file element as the top (root) of this hierarchy
  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "File" );

  // specify the type
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "type" );
  attribute->set_value( "model_solution" );
  top->append( attribute );
 
  // specify the file name
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "name" );
  attribute->set_value( _solution_file );
  top->append( attribute );

  // specify a delimiter for the flat file
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "delimiter" );
  attribute->set_value( delimiter );
  top->append( attribute );

  // add a time field element as a leaf of top
  add_field_element( top, "time", column );

  // iterate over the models and add each component
  for( unsigned i = 0; i < ex_solution->models.size(); i++ ) {
    model_ptr model = ex_solution->models[i];
    
    element = xml_element_ptr( new xml_element_c() );
    element->set_name( "Model" );
    attribute = xml_attribute_ptr( new xml_attribute_c() );
    attribute->set_name( "id" );
    attribute->set_value( model->id );
    element->append( attribute );
    for( unsigned j = 0; j < model->links.size(); j++ ) {
      link_ptr link = model->links[j];
      write_link_element( element, link, column );
    }
    top->append( element );
  }

  // add this hierarchy to the parent node
  parent->append( top );

  // generate the column map
  _solution_column_map->map_solution_element( top );

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write_trial_file_element( xml_element_ptr parent, trial_ptr ex_trial, std::string delimiter ) {
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;
  unsigned column = 1;

  // create the file element as the top (root) of this hierarchy
  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "File" );

  // specify the type
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "type" );
  attribute->set_value( "trial" );
  top->append( attribute );

  // specify the file name
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "name" );
  attribute->set_value( _trial_file );
  top->append( attribute );

  // specify a delimiter for the flat file
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "delimiter" );
  attribute->set_value( delimiter );
  top->append( attribute );

  // add a time field element as a leaf of top
  add_field_element( top, "time", column );

  // iterate over the models and add each component
  for( unsigned i = 0; i < ex_trial->models.size(); i++ ) {
    model_ptr model = ex_trial->models[i];
    
    element = xml_element_ptr( new xml_element_c() );
    element->set_name( "Model" );
    attribute = xml_attribute_ptr( new xml_attribute_c() );
    attribute->set_name( "id" );
    attribute->set_value( model->id );
    element->append( attribute );
    for( unsigned j = 0; j < model->links.size(); j++ ) {
      link_ptr link = model->links[j];
      write_link_element( element, link, column );
    }
    for( unsigned j = 0; j < model->joints.size(); j++ ) {
      joint_ptr joint = model->joints[j];
      write_joint_element( element, joint, column );
    }
    top->append( element );
  }

  // add this hierarchy to the parent node
  parent->append( top );

  // generate the column map
  _trial_column_map->map_trial_element( top );

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write_link_element( xml_element_ptr parent, link_ptr link, unsigned& column ) {
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "Link" );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "id" );
  attribute->set_value( link->id );
  top->append( attribute );
 
  add_field_element( top, "position", column, 3 );

  add_field_element( top, "rotation", column, 4 );

  add_field_element( top, "linear-velocity", column, 3 );

  add_field_element( top, "angular-velocity", column, 3 );

  parent->append( top );

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write_joint_element( xml_element_ptr parent, joint_ptr joint, unsigned& column ) {
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "Joint" );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "id" );
  attribute->set_value( joint->id );
  top->append( attribute );
 
  add_field_element( top, "control", column, 6 );

  parent->append( top );

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::add_column_attribute( xml_element_ptr element, unsigned& column, unsigned size ) {
#ifdef DEFINE_COLUMNS
  xml_attribute_ptr attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "column" );
  attribute->set_value( column );
  element->append( attribute );
  column += size;
#endif
  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::add_field_element( xml_element_ptr parent, std::string name, unsigned& column, unsigned size ) {
  xml_element_ptr element;
  xml_attribute_ptr attribute;

  element = xml_element_ptr( new xml_element_c() );
  element->set_name( "Field" );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "name" );
  attribute->set_value( name );
  element->append( attribute );
  add_column_attribute( element, column, size );
  parent->append( element );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
