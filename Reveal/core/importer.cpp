#include "Reveal/core/importer.h"

#include "Reveal/core/xml.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

importer_c::importer_c( void ) {

}
//-----------------------------------------------------------------------------
importer_c::~importer_c( void ) {

}

//-----------------------------------------------------------------------------
bool importer_c::read_scenario( std::string filename, scenario_ptr& scenario ) {
  scenario = scenario_ptr( new scenario_c() );
  
  xml_c xml;
  xml_element_ptr root, element;
  
  xml.read( filename );
  root = xml.root();

  for( unsigned i = 0; i < root->elements(); i++ ) {
    element = root->element( i );
    if( element->get_name() == "Id" ) {
      scenario->id = element->get_value();
    } else if( element->get_name() == "Description" ) {
      scenario->description = element->get_value();
    } else if( element->get_name() == "Trials" ) {
      scenario->trials = (unsigned)atoi( element->get_value().c_str() );
    } else if( element->get_name() == "Steps_Per_Trial" ) {
      scenario->steps_per_trial = (unsigned)atoi( element->get_value().c_str() );
    } else if( element->get_name() == "URI" ) {
      scenario->uris.push_back( element->get_value() );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
