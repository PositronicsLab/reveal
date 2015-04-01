#include "Reveal/core/datamap.h"

#include <sstream>
#include <string>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

datamap_c::datamap_c( void ) {
  _columns = 0;
}
//-----------------------------------------------------------------------------
datamap_c::~datamap_c( void ) {

}
//-----------------------------------------------------------------------------
bool datamap_c::map_trial_element( xml_element_ptr top ) {
  xml_element_ptr element;
  xml_attribute_ptr attribute;
  std::string key;

  _columns = 0;
  
  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Field" ) {
      map_field_element( element, _columns );
    } else if( element->get_name() == "Model" ) {
      map_model_element( element, _columns );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool datamap_c::map_solution_element( xml_element_ptr top ) {
  xml_element_ptr element;

  _columns = 0;
  
  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Field" ) {
      map_field_element( element, _columns );
    } else if( element->get_name() == "Model" ) {
      map_model_element( element, _columns );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool datamap_c::map_model_element( xml_element_ptr top, unsigned& column ) {
  xml_element_ptr element;
  xml_attribute_ptr attribute;
  std::string key = "";

  for( unsigned i = 0; i < top->attributes(); i++ ) {
    attribute = top->attribute( i );
    if( attribute->get_name() == "id" ) {
      key = attribute->get_value();
      break;
    }
  }
  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Link" )
      map_link_element( element, column, key );
    else if( element->get_name() == "Joint" )
      map_joint_element( element, column, key );
  }

  return true;
}

//-----------------------------------------------------------------------------
bool datamap_c::map_link_element( xml_element_ptr top, unsigned& column, std::string parent_key ) {
  xml_element_ptr element;
  xml_attribute_ptr attribute;
  std::string key = "";

  for( unsigned i = 0; i < top->attributes(); i++ ) {
    attribute = top->attribute( i );
    if( attribute->get_name() == "id" ) {
      key = attribute->get_value();
      break;
    }
  }
  if( parent_key != "" ) {
    key = parent_key + "::" + key;
  }
  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Field" )
      map_field_element( element, column, key );
  }

  return true;
}
//-----------------------------------------------------------------------------
bool datamap_c::map_joint_element( xml_element_ptr top, unsigned& column, std::string parent_key ) {
  xml_element_ptr element;
  xml_attribute_ptr attribute;
  std::string key = "";

  for( unsigned i = 0; i < top->attributes(); i++ ) {
    attribute = top->attribute( i );
    if( attribute->get_name() == "id" ) {
      key = attribute->get_value();
      break;
    }
  }
  if( parent_key != "" ) {
    key = parent_key + "::" + key;
  }
  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    if( element->get_name() == "Field" )
      map_field_element( element, column, key );
  }

  return true;
}
//-----------------------------------------------------------------------------
bool datamap_c::map_field_element( xml_element_ptr element, unsigned& column, std::string parent_key ) {
  xml_attribute_ptr attribute;
  std::string key = "";
  unsigned size = 1;

  // This method needs more flexibility.  Parsing the field here makes it easy
  // to know size, but the hardcoding of field names limits the capability
  for( unsigned j = 0; j < element->attributes(); j++ ) {
    attribute = element->attribute( j );
    if( attribute->get_name() == "name" ) {
      key += attribute->get_value();

      if( key == "position" ) {
        size = 3;
      } else if( key == "rotation" ) {
        size = 4;
      } else if( key == "linear-velocity" ) {
        size = 3;
      } else if( key == "angular-velocity" ) {
        size = 3;
      } else if( key == "control" ) {
        size = 6;
      }

      if( parent_key != "" ) {
        key = parent_key + "::" + key;
      }
      _map.insert( std::pair<std::string,unsigned>( key, ++column ) );
      column += (size - 1);
      break;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
void datamap_c::print( void ) {
  for( std::map<std::string,unsigned>::iterator it = _map.begin(); it != _map.end(); it++ ) {
    std::cout << it->first << "," << it->second << std::endl;
  }
}

//-----------------------------------------------------------------------------
unsigned datamap_c::columns( void ) {
  return _columns;
}
//-----------------------------------------------------------------------------
unsigned datamap_c::column( std::string key ) {
  std::map<std::string, unsigned>::iterator it = _map.find( key );
  if( it == _map.end() ) return 0;
  return it->second;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

datawriter_c::datawriter_c( void ) {
  _open = false;
  _filename = "";
}
//-----------------------------------------------------------------------------
datawriter_c::datawriter_c( std::string filename, std::string delimiter, datamap_ptr column_map ) {
  _open = false;
  _filename = filename;
  _column_map = column_map;
  _cells.resize( column_map->columns() );
  _delimiter = delimiter;
}
//-----------------------------------------------------------------------------
datawriter_c::~datawriter_c( void ) {
  if( _open ) {
    flush();
    close();
  }
}
//-----------------------------------------------------------------------------
bool datawriter_c::open( void ) {
  _outfile = new std::ofstream( _filename.c_str() );
  _open = true;

  return _open;
}

//-----------------------------------------------------------------------------
void datawriter_c::close( void ) {
  _outfile->close();
  delete _outfile;
  _open = false;
}

//-----------------------------------------------------------------------------
bool datawriter_c::write( std::string key, double value, unsigned offset ) {
  unsigned column = _column_map->column( key );
  if( column == 0 ) {
    printf( "column returned zero\n" );
    return false;
  }
  
  column += offset;
  if( column > _column_map->columns() ) return false;

  _cells[column-1] = value;

  return true;
}

//-----------------------------------------------------------------------------
void datawriter_c::flush( void ) {
  assert( _open );
  // write to stream
  std::stringstream ss;
  
  unsigned i = 0;
  for( std::vector<double>::iterator it = _cells.begin(); it != _cells.end(); it++ ) {
    if( i++ > 0 ) ss << _delimiter;
    ss << (*it);
    // reset the value to zero after flushing s.t. data doesn't hang around.
    (*it) = 0.0;
  }
  ss << std::endl;

  std::string s = ss.str();
  _outfile->write( s.c_str(), s.size() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

datareader_c::datareader_c( void ) {

}

//-----------------------------------------------------------------------------
datareader_c::datareader_c( std::string filename, std::string delimiter, datamap_ptr column_map ) {
  _open = false;
  _filename = filename;
  _column_map = column_map;
  _cells.resize( column_map->columns() );
  _delimiter = delimiter;
}
//-----------------------------------------------------------------------------
datareader_c::~datareader_c( void ) {
  if( _open ) {
    close();
  }
}

//-----------------------------------------------------------------------------
bool datareader_c::open( void ) {
  _infile = new std::ifstream( _filename.c_str() );
  if( _infile->is_open() )
    _open = true;

  return _open;
}

//-----------------------------------------------------------------------------
void datareader_c::close( void ) {
  if( _open ) {
    _infile->close();
    delete _infile;
  }
  _open = false;
}

//-----------------------------------------------------------------------------
bool datareader_c::buffer_line( void ) {
  if( !_open ) return false;

  std::string line;
  if( !std::getline( *_infile, line ) ) 
    return false;

  std::stringstream buffer( line );
  std::string token;
  const char delimiter = (const char)_delimiter[0];
  while( std::getline( buffer, token, delimiter ) ) {
    unsigned i = 0;
    _cells[i++] = atof( token.c_str() );
    while( buffer >> _cells[i++] );
  }
  return true;
}

//-----------------------------------------------------------------------------
void datareader_c::print_cells( void ) {
  for( unsigned i = 0; i < _cells.size(); i++ ) {
    std::cout << _cells[i] << " ";
  }
  std::cout << std::endl;
}

//-----------------------------------------------------------------------------
bool datareader_c::read( trial_ptr& trial, std::string scenario_id, unsigned index, xml_element_ptr top ) {
  if( !buffer_line() ) return false;
  //print_cells();

  trial = trial_ptr( new trial_c() );
  trial->trial_id = index;
  trial->scenario_id = scenario_id;
  component_ptr component = boost::dynamic_pointer_cast<trial_c>( trial );

  xml_element_ptr element;
  std::string key, name;

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    name = element->get_name();
    if( name == "Field" ) {
      //printf( "here\n" );
      read_field( component, element );
    } else if( name == "Model" ) {
      read_model( component, element );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool datareader_c::read( solution_ptr& solution, std::string scenario_id, unsigned index, xml_element_ptr top ) {
  if( !buffer_line() ) return false;
  //print_cells();

  solution = solution_ptr( new solution_c( solution_c::MODEL ) );
  solution->scenario_id = scenario_id;
  solution->trial_id = index;
  component_ptr component = boost::dynamic_pointer_cast<solution_c>( solution );

  xml_element_ptr element;
  std::string key, tag;

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    tag = element->get_name();
    if( tag == "Field" ) {
      read_field( component, element );
    } else if( tag == "Model" ) {
      read_model( component, element );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool datareader_c::read_model( component_ptr owner, xml_element_ptr top, std::string key ) {
  xml_attribute_ptr attribute = top->attribute( "id" );
  if( !attribute ) return false;

  std::string name = attribute->get_value();
  xml_element_ptr element;
  std::string tag;

  model_ptr model = model_ptr( new model_c() );
  if( owner->component_type() == component_c::TRIAL ) {
    trial_ptr trial = boost::dynamic_pointer_cast<trial_c>( owner );
    trial->models.push_back( model );
  } else if( owner->component_type() == component_c::SOLUTION ) {
    solution_ptr solution = boost::dynamic_pointer_cast<solution_c>( owner );
    solution->models.push_back( model );
  } else {
    return false;
  }

  key = name;
  model->id = name;

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    tag = element->get_name();
    if( tag == "Link" ) {
      read_link( model, element, key );
    } else if( tag == "Joint" ) {
      read_joint( model, element, key );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
bool datareader_c::read_link( model_ptr owner, xml_element_ptr top, std::string key ) {
  xml_attribute_ptr attribute = top->attribute( "id" );
  if( !attribute ) return false;

  std::string name = attribute->get_value();
  xml_element_ptr element;
  std::string tag;

  link_ptr link = link_ptr( new link_c() );
  component_ptr component = boost::dynamic_pointer_cast<component_c>( link );

  if( key != "" )
    key += "::"; 
  key += name;
  link->id = name;

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    tag = element->get_name();
    if( tag == "Field" ) {
      read_field( component, element, key );
    }
  }

  owner->insert( link );

  return true;
}

//-----------------------------------------------------------------------------
bool datareader_c::read_joint( model_ptr owner, xml_element_ptr top, std::string key ) {
  xml_attribute_ptr attribute = top->attribute( "id" );
  if( !attribute ) return false;

  std::string name = attribute->get_value();
  xml_element_ptr element;
  std::string tag;

  joint_ptr joint = joint_ptr( new joint_c() );
  component_ptr component = boost::dynamic_pointer_cast<component_c>( joint );

  if( key != "" )
    key += "::";
  key += name;
  joint->id = name;

  for( unsigned i = 0; i < top->elements(); i++ ) {
    element = top->element( i );
    tag = element->get_name();
    if( tag == "Field" ) {
      read_field( component, element, key );
    }
  }

  owner->insert( joint );

  return true;
}

//-----------------------------------------------------------------------------
bool datareader_c::read_field( component_ptr owner, xml_element_ptr top, std::string key ) {

  xml_attribute_ptr attribute = top->attribute( "name" );
  if( !attribute ) return false;

  std::string name = attribute->get_value();
//  printf( "name: %s\n", name.c_str() ); 

  unsigned size = 1;
  if( key != "" )
    key += "::"; 
  key += name;
 
  unsigned column = _column_map->column( key );
  //printf( "key[%s], column[%u]\n", key.c_str(), column );

  if( owner->component_type() == component_c::TRIAL ) {
    trial_ptr trial = boost::dynamic_pointer_cast<trial_c>( owner );
    if( name == "time" ) {
      trial->t = _cells[column-1];
      return true;
    } else if( name == "time-step" ) {
      trial->dt = _cells[column-1];
      return true;
    }
  } else if( owner->component_type() == component_c::SOLUTION ) {
    solution_ptr solution = boost::dynamic_pointer_cast<solution_c>( owner );
    if( name == "time" ) {
      solution->t = _cells[column-1];
      return true;
    } else if( name == "time-step" ) {
      solution->dt = _cells[column-1];
      return true;
    }
  } else if( owner->component_type() == component_c::LINK ) {
    link_ptr link = boost::dynamic_pointer_cast<link_c>( owner );
    if( name == "position" ) {
      size = 3;
      link->state.linear_x( _cells[column-1] );
      link->state.linear_y( _cells[column] );
      link->state.linear_z( _cells[column+1] );
      return true;
    } else if( name == "rotation" ) {
      size = 4;
      link->state.angular_x( _cells[column-1] );
      link->state.angular_y( _cells[column] );
      link->state.angular_z( _cells[column+1] );
      link->state.angular_w( _cells[column+2] );
      return true;
    } else if( name == "linear-velocity" ) {
      size = 3;
      link->state.linear_dx( _cells[column-1] );
      link->state.linear_dy( _cells[column] );
      link->state.linear_dz( _cells[column+1] );
      return true;
    } else if( name == "angular-velocity" ) {
      size = 3;
      link->state.angular_dx( _cells[column-1] );
      link->state.angular_dy( _cells[column] );
      link->state.angular_dz( _cells[column+1] );
      return true;
    }
  } else if( owner->component_type() == component_c::JOINT ) {
    joint_ptr joint = boost::dynamic_pointer_cast<joint_c>( owner );
    if( name == "control" ) {
      size = 6;
      for( unsigned i = 0; i < size; i++ ) {
        unsigned cell = column - 1 + i;
        joint->control[i] = _cells[cell];
      }
      return true; 
    }
  } 

  return false;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
