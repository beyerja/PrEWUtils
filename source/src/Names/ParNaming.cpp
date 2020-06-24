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

std::string ParNaming::total_chi_xs_par_name(const std::string & distr_name) {
  /** Convention for naming the parameter corresponding to the scaling of the
      sum of the chiral cross sections for a given distribution.
  **/
  return "ScaleTotChiXS_" + distr_name;
}

std::string ParNaming::asymm_par_name ( 
  const std::string & distr_name,
  int asymm_index 
) {
  /** Convention for naming the parameters corresponding to asymmetries for a 
      given distribution.
      Contains an index in case more than one symmetry is allowed.
  **/
  std::string par_name = "DeltaA";
  
  // Figure out which asymmetry parameter is meant
  switch(asymm_index) {
    case 0 : break;
    case 1 : par_name += "_I";   break;
    case 2 : par_name += "_II";  break;
    case 3 : par_name += "_III"; break;
    default: 
      spdlog::error("ParNaming: Unknown asymmetry index {}!", asymm_index);
      return "";
  }
  par_name += "_" + distr_name;
  return par_name;
}

//------------------------------------------------------------------------------

std::string ParNaming::Af_par_name ( const std::string & distr_name ) {
  /** Convention on how to name the (Delta-)Af parameter for a 2-fermion
      distribution.
  **/
  return "Af_" + distr_name;
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