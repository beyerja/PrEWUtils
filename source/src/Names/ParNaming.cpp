#include <Names/ParNaming.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace Names {

//------------------------------------------------------------------------------

std::string ParNaming::chi_xs_par_name(
  const std::string & distr_name,
  const std::string & chiral_config
) {
  /** Convention for the naming of the parameter corresponding to a total chiral
      cross section scaling for a given distribution.
  **/
  std::string internal_config_name = {};
  if (chiral_naming_map.find(chiral_config) != chiral_naming_map.end()) {
    internal_config_name = chiral_naming_map.at(chiral_config);
  } else {
    spdlog::warn( 
      "Unknown chiral config: {} -> Cross section not free!", chiral_config
    );
  }
  
  return "ChiXS_" + distr_name + "_" + internal_config_name;
}
  
//------------------------------------------------------------------------------

std::string ParNaming::lumi_name ( int energy, std::string energy_unit ) {
  /** Convention for naming the luminosity parameter.
  **/
  return "Lumi" + std::to_string(energy) + energy_unit;
}

//------------------------------------------------------------------------------

} // Namespace Names
} // Namespace PrEWUtils