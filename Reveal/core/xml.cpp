#include "Reveal/core/xml.h"

#include <tinyxml.h>

#include <assert.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

xml_attribute_c::xml_attribute_c( void ) { 
  _name = "";
  _value = "";
}
//-----------------------------------------------------------------------------
xml_attribute_c::~xml_attribute_c( void ) {

}
//-----------------------------------------------------------------------------
xml_attribute_ptr xml_attribute_c::ptr( void ) { 
  return shared_from_this();
}

//-----------------------------------------------------------------------------
std::string xml_attribute_c::get_name( void ) {
  return _name;
}
//-----------------------------------------------------------------------------
void xml_attribute_c::set_name( std::string name ) {
  _name = name;
}
//-----------------------------------------------------------------------------
std::string xml_attribute_c::get_value( void ) {
  return _value;
}
//-----------------------------------------------------------------------------
void xml_attribute_c::set_value( std::string value ) {
  _value = value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
xml_element_c::xml_element_c( void ) {
  _value = "";
}
//-----------------------------------------------------------------------------
xml_element_c::~xml_element_c( void ) { }
//-----------------------------------------------------------------------------
xml_element_ptr xml_element_c::ptr( void ) { 
  return shared_from_this();
}
//-----------------------------------------------------------------------------
unsigned xml_element_c::attributes( void ) {
  return _attributes.size();
}
//-----------------------------------------------------------------------------
xml_attribute_ptr xml_element_c::attribute( unsigned i ) {
  assert( i < _attributes.size() );
  return _attributes[i];
}
//-----------------------------------------------------------------------------
void xml_element_c::append( xml_attribute_ptr attribute ) {
  _attributes.push_back( attribute );
}

//-----------------------------------------------------------------------------
unsigned xml_element_c::elements( void ) {
  return _elements.size();
}
//-----------------------------------------------------------------------------
xml_element_ptr xml_element_c::element( unsigned i ) {
  assert( i < _elements.size() );
  return _elements[i];
}
//-----------------------------------------------------------------------------
void xml_element_c::append( xml_element_ptr element ) {
  _elements.push_back( element );
}
//-----------------------------------------------------------------------------
std::string xml_element_c::get_name( void ) {
  return _name;
}
//-----------------------------------------------------------------------------
void xml_element_c::set_name( std::string name ) {
  _name = name;
}
//-----------------------------------------------------------------------------
std::string xml_element_c::get_value( void ) {
  return _value;
}
//-----------------------------------------------------------------------------
void xml_element_c::set_value( std::string value ) {
  _value = value;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
xml_c::xml_c( void ) { }
//-----------------------------------------------------------------------------
xml_c::~xml_c( void ) { }
//-----------------------------------------------------------------------------
bool xml_c::read( std::string filename ) {
  // TODO: robust error checking

  TiXmlDocument doc;
  doc = TiXmlDocument( filename );
  
  bool result;
  result = doc.LoadFile();
  if( !result ) {
    return false;
  }

  TiXmlHandle tixml_hdoc( &doc );
  TiXmlHandle tixml_root( 0 ); 
  TiXmlElement* tixml_e;
  TiXmlHandle tixml_h( 0 );

  tixml_e = tixml_hdoc.FirstChildElement().Element();
  tixml_root = TiXmlHandle( tixml_e );

  _root = xml_element_ptr( new xml_element_c() );
  process_tixml_element( (void*)tixml_e, _root );

  return true;
}

//-----------------------------------------------------------------------------
bool xml_c::write( std::string filename ) {
  // TODO: robust error checking

  TiXmlDocument doc;
  TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
  doc.LinkEndChild( decl );

  TiXmlElement* tixml_root = new TiXmlElement( _root->get_name() );
  doc.LinkEndChild( tixml_root );

  build_tixml_element( tixml_root, _root );

  doc.SaveFile( filename );

  return true;
}

//-----------------------------------------------------------------------------
void xml_c::root( xml_element_ptr element ) {
  _root = element;
}

//-----------------------------------------------------------------------------
xml_element_ptr xml_c::root( void ) {
  return _root;
}

//-----------------------------------------------------------------------------
bool xml_c::build_tixml_element( void* tixml_element, xml_element_ptr element ) {
  // TODO: robust error checking

  TiXmlElement* tixml_e = (TiXmlElement*) tixml_element;
  std::string value = element->get_value();
  if( value != "" )
    tixml_e->LinkEndChild( new TiXmlText( value ) );

  for( unsigned i = 0; i < element->attributes(); i++ ) {
    xml_attribute_ptr attrib = element->attribute( i );
    tixml_e->SetAttribute( attrib->get_name(), attrib->get_value() );
  }

  for( unsigned i = 0; i < element->elements(); i++ ) {
    xml_element_ptr child = element->element( i );
    TiXmlElement* tixml_child = new TiXmlElement( child->get_name() );
    tixml_e->LinkEndChild( tixml_child );
    build_tixml_element( tixml_child, child ); 
  }

  return true;
}
//-----------------------------------------------------------------------------
bool xml_c::process_tixml_element( void* tixml_element, xml_element_ptr element ) {
  // TODO: robust error checking
 
  TiXmlElement* tixml_e = (TiXmlElement*) tixml_element;
  const char* pkey = tixml_e->Value();
  const char* ptext = tixml_e->GetText();

  element->set_name( pkey );
  if( ptext ) 
    element->set_value( ptext );

  TiXmlAttribute* a = tixml_e->FirstAttribute();
  while( a!= NULL ) {
    std::string name = a->Name();
    std::string value = a->Value();
    
    xml_attribute_ptr attrib = xml_attribute_ptr( new xml_attribute_c() );
    attrib->set_name( a->Name() );
    attrib->set_value( a->Value() );
    element->append( attrib );

    a = a->Next();
  }

  TiXmlElement* tixml_child;

  for( tixml_child = tixml_e->FirstChildElement(); tixml_child != NULL; tixml_child = tixml_child->NextSiblingElement() ) {
    xml_element_ptr child = xml_element_ptr( new xml_element_c() );
    element->append( child );
    process_tixml_element( tixml_child, child );
  }
 

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
