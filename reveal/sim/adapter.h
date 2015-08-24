#ifndef _REVEAL_SIM_ADAPTER_H_
#define _REVEAL_SIM_ADAPTER_H_

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
namespace Reveal {
//-----------------------------------------------------------------------------
namespace Sim {
//-----------------------------------------------------------------------------

class sim_interface_c {
public:
  std::string id;
  std::string name;
  std::string source_path;
  std::string build_path;
  std::string driver_path;
  std::string library_path;
  std::string library_name;

  sim_interface_c( void ) {
    id = "";
    name = "";
    source_path = "";
    build_path = "";
    driver_path = "";
    library_path = "";
    library_name = "";
  }

  virtual ~sim_interface_c( void ) { }
};

class adapter_c {
public:
  std::vector<sim_interface_c> interfaces;

  adapter_c( void );
  virtual ~adapter_c( void );

  bool identify_sims( void );
  bool compile_interface( sim_interface_c& interface );
};

//-----------------------------------------------------------------------------
} // namespace Sim
//-----------------------------------------------------------------------------
} // namespace Reveal
//-----------------------------------------------------------------------------
#endif // _REVEAL_SIM_ADAPTER_H_
