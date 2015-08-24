#include "reveal/sim/adapter.h"

#include "reveal/core/system.h"
#include "reveal/core/xml.h"
#include "reveal/core/package.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Sim {
//-----------------------------------------------------------------------------
adapter_c::adapter_c( void ) {

}

//-----------------------------------------------------------------------------
adapter_c::~adapter_c( void ) {

}

//-----------------------------------------------------------------------------
bool adapter_c::identify_sims( void ) {
  Reveal::Core::system_c system( Reveal::Core::system_c::SERVER );
  if( !system.open() ) return false;

  // get the path to where reveal_sims source is maintained
  std::string reveal_sims_path = system.sims_path();
  // get the search paths to where simulators may reside
  std::string reveal_sims_bin_search_path = system.sims_bin_search_path();
  std::string reveal_sims_lib_search_path = system.sims_lib_search_path();

  system.close();

  // break the search path variables into individual paths
  std::vector<std::string> bin_search_paths;
  std::vector<std::string> lib_search_paths;

  bin_search_paths = split_multipath_string( reveal_sims_bin_search_path );
  lib_search_paths = split_multipath_string( reveal_sims_lib_search_path );

  // validate presence of reveal_sims
  if( !path_exists( reveal_sims_path ) || !is_directory( reveal_sims_path ) ) {
    // the path specified to reveal sims does not exist.  report and bomb out
    printf( "ERROR: reveal_sims is not present in the specified location[%s]\n", reveal_sims_path.c_str() );
    return false;
  }

  // build the expected path to decend into the source tree
  std::string path = combine_path( reveal_sims_path, "reveal" );
  path = combine_path( path, "sim" );

  // validate structure of reveal_sims
  if( !path_exists( path ) || !is_directory(path) ) {
    printf( "ERROR: reveal_sims does not have the correct structure\n" );
    return false;
  }

  // get the subdirectories in reveal_sims as each one identifies a different
  // simulator interface that reveal supports
  std::vector<std::string> iface_paths = get_subdirectories( path );
  if( !iface_paths.size() ) {
    //the directory does not contain any simulator interfaces.  report and bomb
    printf( "ERROR: there are no simulator interfaces in reveal_sims\n" );
    return false;
  }

  // iterate over all the available interfaces and read their requirements 
  for( std::vector<std::string>::iterator it = iface_paths.begin(); it != iface_paths.end(); it++ ) {
    // read the definition xml file in each subdirectory to identify the search
    // targets
    Reveal::Core::xml_c xml;
    Reveal::Core::xml_element_ptr root, element;
    Reveal::Core::xml_attribute_ptr attribute;

    std::string path_to_reqs = combine_path( *it, "reqs.xml" );
    if( !xml.read( path_to_reqs ) ) {
      // this sim interface is incomplete and cannot continue to process it
      printf( "encountered an incomplete simulator interface[%s].  Bypassing\n", it->c_str() );
      continue;
    }

    // get the xml root
    root = xml.root();
    bool found_definition = false, found_requirement = false;

    std::string sim_id = "", sim_name = "", sim_bin = "", sim_lib = "";

    // parse the rest of the xml file
    for( unsigned i = 0; i < root->elements(); i++ ) {
      element = root->element(i);

      if( element->get_name() == "Sim" ) {
        for( unsigned j = 0; j < element->attributes(); j++ ) {
          attribute = element->attribute( j );

          if( attribute->get_name() == "id" ) {
            sim_id = attribute->get_value();
          } else if( attribute->get_name() == "name" ) {
            sim_name = attribute->get_value();
          } else if( attribute->get_name() == "bin" ) {
            sim_bin = attribute->get_value();
          } else if( attribute->get_name() == "lib" ) {
            sim_lib = attribute->get_value();
          }
        }

        //printf( "read requirements: id[%s], name[%s], bin[%s], lib[%s]\n", sim_id.c_str(), sim_name.c_str(), sim_bin.c_str(), sim_lib.c_str() );
        if( sim_id != "" && sim_name != "" && ( sim_bin != "" || sim_lib != "" ) ) {
          found_definition = true;  // may need more validation
        }
      }
    }
    // if a simulator definition was not found, then move on to next candidate
    if( !found_definition ) {
      printf( "failed to find simulator definition\n" );
      continue;
    }

    // otherwise, scan the system for the simulator's requirements

    if( sim_bin != "" ) {
      // if the definition references a bin file...
      for( std::vector<std::string>::iterator pit = bin_search_paths.begin(); pit != bin_search_paths.end(); pit++ ) {
        std::string bin_path;
        if( find_file( bin_path, sim_bin, *pit ) ) {
          found_requirement = true;
          break;
        }
      }
    } else if( sim_lib != "" ) {
      // if the definition references a lib file...
      for( std::vector<std::string>::iterator pit = lib_search_paths.begin(); pit != lib_search_paths.end(); pit++ ) {
        std::string lib_path;
        if( find_file( lib_path, sim_bin, *pit ) ) {
          found_requirement = true;
          break;
        }
      }
    }

    // if the required simulator file was not found then cannot use this sim
    if( !found_requirement ) {
      printf( "failed to find simulator requirement\n" );
      continue;
    }

    // otherwise add this sim to the list of available simulators
    //std::string path_to_interface = *it;
    //std::string simulator_name = sim_name;

    sim_interface_c interface;
    interface.id = sim_id;
    interface.name = sim_name;
    interface.source_path = *it;

    interfaces.push_back( interface );
  }


  // for each search target, attempt to locate the binary file.

  // if the binary file is found then add that simulator to the list of 
  // available sims

  // otherwise ignore the simulator

  // NOTE: some simulators are bin based while others are lib based.  Have to be
  // support searching for both cases.
  // bin files should be identifiable through PATH environment variable
  // lib files might be identifiable through LD_LIBRARY_PATH env variable but 


  return true;
}

//-----------------------------------------------------------------------------
bool adapter_c::compile_interface( sim_interface_c& interface ) {
  // create a build directory
  // build the sim interface

  assert( interface.source_path != "" );

  if( !path_exists( interface.source_path ) || !is_directory( interface.source_path ) ) {
    // the interface source path does not point to a valid location
    return false;
  }

  if( interface.build_path == "" )
    interface.build_path = combine_path( interface.source_path, "build" );

  if( !get_directory( interface.build_path ) ) {
    // system is unable to create a build directory or to access an existing one
    return false;
  }
 
  // otherwise we have a valid source and build path

  // call the package builder to construct the simulator
  // Note: POSIX
  std::string driver_name = "libreveal_sim_";
  driver_name += interface.id + "_driver.so";

  std::string library_name = "libreveal_sim_";
  library_name += interface.id + ".so";

//  std::string build_product = "libreveal_sim_";
//  build_product += interface.id + "_driver.so";

  std::vector<std::string> build_products;
  //build_products.push_back( build_product );
  build_products.push_back( driver_name );
  build_products.push_back( library_name );
  // monitor?  // Note: this is specialized to gazebo.  need to extend xml

  Reveal::Core::package_ptr package( new Reveal::Core::package_c( interface.source_path, interface.build_path ) );

  if( !package->configure() ) {
    return false;
  }

  if( !package->make( build_products ) ) {
    return false;
  }
 
  interface.driver_path = combine_path( interface.build_path, driver_name );

  //printf( "interface.driver_path = %s\n", interface.driver_path.c_str() );

  interface.library_path = interface.build_path;
  interface.library_name = library_name;
 
  return true;
}

//-----------------------------------------------------------------------------
} // namespace Sim
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
