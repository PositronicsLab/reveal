#include "Reveal/core/exporter.h"

#include "Reveal/core/xml.h"

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

//bool exporter_c::write_scenario( scenario_ptr scenario, std::string filename, std::vector<std::string> data_map ) {
bool exporter_c::write_scenario( scenario_ptr scenario, std::string filename ) {
  xml_c xml;
  xml_element_ptr root, element;
  root = xml_element_ptr( new xml_element_c() );
  root->set_name( "Scenario" );

  element = xml_element_ptr( new xml_element_c() );
  element->set_name( "Id" );
  element->set_value( scenario->id );
  root->append( element );

  element = xml_element_ptr( new xml_element_c() );
  element->set_name( "Description" );
  element->set_value( scenario->description );
  root->append( element );

  element = xml_element_ptr( new xml_element_c() );
  element->set_name( "Trials" );
  element->set_value( scenario->trials );
  root->append( element );

  element = xml_element_ptr( new xml_element_c() );
  element->set_name( "Steps_Per_Trial" );
  element->set_value( scenario->steps_per_trial );
  root->append( element );

  for( unsigned i = 0; i < scenario->uris.size(); i++ ) {
    element = xml_element_ptr( new xml_element_c() );
    element->set_name( "URI" );
    element->set_value( scenario->uris[i] );
    root->append( element );
  }

  xml.root( root );
  xml.write( filename );

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
