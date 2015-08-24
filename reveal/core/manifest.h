/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

manifest.h defines the manifest_c class that reads a manifest xml file from
a package and publishes the manifest tree typically for package building.
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_MANIFEST_H_
#define _REVEAL_CORE_MANIFEST_H_
//-----------------------------------------------------------------------------

#include <vector>
#include <string>

#include "reveal/core/xml.h"

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

/**
Reads a manifest xml file from a package and returns the manifest xml tree
*/
class manifest_c {
public:
  /// Reads the manifest file and locates the manifest xml element.
  /// @param package_path the path to directory containing the package
  /// @return a valid xml_element_ptr OR an empty pointer
  static xml_element_ptr read( std::string package_path ) {
    xml_element_ptr root, manifest, element;
    std::string manifest_path = package_path + '/'+ "manifest.xml";

    xml_c xml;
    if( !xml.read( manifest_path ) ) return manifest;

    root = xml.root();

    // find the manifest element
    for( unsigned i = 0; i < root->elements(); i++ ) {
      element = root->element( i );
      if( element->get_name() == "Manifest" ) {
        manifest = element;
        break;
      }
    }
    return manifest;
  }
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
#endif // _REVEAL_CORE_MANIFEST_H_
