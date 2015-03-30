#include "Reveal/core/datamap.h"

#include <sstream>

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

  _columns = 1;
  
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

  _columns = 1;
  
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
      _map.insert( std::pair<std::string,unsigned>( key, column ) );
      column += size;
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


//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
