#include "Reveal/core/exporter.h"

#include <sstream>

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

  std::stringstream ss_scenario_file, ss_trial_file, ss_solution_file;
  ss_scenario_file << scenario->id << ".scenario";
  _scenario_file = ss_scenario_file.str();
  ss_trial_file << scenario->id << ".trials";
  _trial_file = ss_trial_file.str();
  ss_solution_file << scenario->id << ".solutions";
  _solution_file = ss_solution_file.str();

  root = xml_element_ptr( new xml_element_c() );
  root->set_name( "Reveal" );

  _trial_column_map = datamap_ptr( new datamap_c() );
  _solution_column_map = datamap_ptr( new datamap_c() );

  write_scenario_element( root, scenario, analyzer, ex_trial, ex_solution, delimiter );

  xml.root( root );
  xml.write( _scenario_file );

  _trial_column_map->print();
  _solution_column_map->print();

  _trial_datawriter = Reveal::Core::datawriter_c( _trial_file, delimiter, _trial_column_map );
  _solution_datawriter = Reveal::Core::datawriter_c( _solution_file, delimiter, _solution_column_map );

  // TODO error handling
  _trial_datawriter.open();
  _solution_datawriter.open();

  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::write( analyzer_ptr analyzer ) {
  xml_c xml;
  xml_element_ptr root;

  std::stringstream ss_analyzer_file;
  ss_analyzer_file << analyzer->scenario_id << ".analyzer";

  root = xml_element_ptr( new xml_element_c() );
  root->set_name( "Reveal" );

  write_analyzer_element( root, analyzer, false );

  xml.root( root );
  xml.write( ss_analyzer_file.str() );

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

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "Scenario" );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "id" );
  attribute->set_value( scenario->id );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "trials" );
  attribute->set_value( scenario->trials );
  top->append( attribute );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "steps-per-trial" );
  attribute->set_value( scenario->steps_per_trial );
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

  write_analyzer_element( top, analyzer );
  write_trial_element( top, trial, scenario->id, delimiter );
  write_solution_element( top, solution, scenario->id, delimiter );

  parent->append( top );

  return true;
}
//-----------------------------------------------------------------------------
bool exporter_c::write_analyzer_element( xml_element_ptr parent, analyzer_ptr analyzer, bool reference_only ) {
  xml_c xml;
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;

  // Note: really only should have to specify the filename for scenario 
  // generation.  The rest of the record needs to be exported in a separate 
  // framework as the server needs to validate the pathing and the typing
  // and it requires a compilation step on the server anyway.

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "Analyzer" );

  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "id" );
  attribute->set_value( analyzer->scenario_id );
  top->append( attribute );

  if( !reference_only ) {
    attribute = xml_attribute_ptr( new xml_attribute_c() );
    attribute->set_name( "type" );
    if( analyzer->type == Reveal::Core::analyzer_c::PLUGIN ) 
      attribute->set_value( "plugin" );
    else 
      attribute->set_value( "script" );
    top->append( attribute );

    attribute = xml_attribute_ptr( new xml_attribute_c() );
    attribute->set_name( "file" );
    attribute->set_value( analyzer->filename );
    top->append( attribute );
  }

  parent->append( top );

  return true;
}
//-----------------------------------------------------------------------------
bool exporter_c::write_solution_element( xml_element_ptr parent, solution_ptr ex_solution, std::string scenario_id, std::string delimiter ) {
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;
  unsigned column = 1;

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "File" );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "type" );
  attribute->set_value( "model_solution" );
  top->append( attribute );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "delimiter" );
  attribute->set_value( delimiter );
  top->append( attribute );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "name" );
  std::stringstream filename;
  filename << scenario_id << ".solutions";
  attribute->set_value( filename.str() );
  top->append( attribute );

  add_field_element( top, "time", column );

  add_field_element( top, "time-step", column );

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

  parent->append( top );

  _solution_column_map->map_solution_element( top );

  return true;
}
//-----------------------------------------------------------------------------
bool exporter_c::write_trial_element( xml_element_ptr parent, trial_ptr ex_trial, std::string scenario_id, std::string delimiter ) {
  xml_element_ptr top, element;
  xml_attribute_ptr attribute;
  unsigned column = 1;

  top = xml_element_ptr( new xml_element_c() );
  top->set_name( "File" );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "type" );
  attribute->set_value( "trial" );
  top->append( attribute );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "delimiter" );
  attribute->set_value( delimiter );
  top->append( attribute );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "name" );
  std::stringstream filename;
  filename << scenario_id << ".trials";
  attribute->set_value( filename.str() );
  top->append( attribute );

  add_field_element( top, "time", column );

  add_field_element( top, "time-step", column );

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

  parent->append( top );

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
 
  add_field_element( top, "position", column, 3, "x y z" );

  add_field_element( top, "rotation", column, 4, "x y z w" );

  add_field_element( top, "linear-velocity", column, 3, "x y z" );

  add_field_element( top, "angular-velocity", column, 3, "x y z" );

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
 
  add_field_element( top, "control", column, 1, "u" );

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
bool exporter_c::add_map_attribute( xml_element_ptr element, std::string map ) {
#ifdef DEFINE_MAP
  xml_attribute_ptr attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "map" );
  attribute->set_value( map );
  element->append( attribute ); 
#endif
  return true;
}

//-----------------------------------------------------------------------------
bool exporter_c::add_field_element( xml_element_ptr parent, std::string name, unsigned& column, unsigned size, std::string map ) {
  xml_element_ptr element;
  xml_attribute_ptr attribute;

  element = xml_element_ptr( new xml_element_c() );
  element->set_name( "Field" );
  attribute = xml_attribute_ptr( new xml_attribute_c() );
  attribute->set_name( "name" );
  attribute->set_value( name );
  element->append( attribute );
  add_column_attribute( element, column, size );
  if( map != "" )
    add_map_attribute( element, map );
  parent->append( element );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
