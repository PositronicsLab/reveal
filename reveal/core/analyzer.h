/*------------------------------------------------------------------------------
author: James R Taylor (jrt@gwu.edu)

analyzer.h defines the analyzer_c interface that defines custom analytic
programs to run on a specified scenario
------------------------------------------------------------------------------*/

#ifndef _REVEAL_CORE_ANALYZER_H_
#define _REVEAL_CORE_ANALYZER_H_

//-----------------------------------------------------------------------------

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Core {
//-----------------------------------------------------------------------------

class analyzer_c;
typedef boost::shared_ptr<Reveal::Core::analyzer_c> analyzer_ptr;

//-----------------------------------------------------------------------------

class analyzer_c {
public:
  /// The possible types of analyzers
  enum type_e {
    PLUGIN,         
    SCRIPT           
  };

  /// Default constructor
  analyzer_c( void ) { }
  /// Destructor
  virtual ~analyzer_c( void ) { }

  std::string            scenario_id;    //< unique scenario identifier
  std::string            analyzer_id;    //< unique analyzer identifier
  std::string            source_path;    //< the path to the analyzers source
  std::string            build_path;     //< the path to the analyzers build dir
  std::string            build_target;   //< the build target name for a plugin
  type_e 	         type;           //< the type of analyzer

  std::vector<std::string>  keys;   //< the set of keys used for database search
  std::vector<std::string>  labels; //< the labels to use when displaying keys
};

//-----------------------------------------------------------------------------
} // namespace Core
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------

#endif // _REVEAL_CORE_ANALYZER_H_
