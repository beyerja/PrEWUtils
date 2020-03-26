#ifndef LIB_PARNAMING_H
#define LIB_PARNAMING_H 1

// Includes from PrEW
#include <GlobalVar/Chiral.h>

#include <string>
#include <map>

namespace PrEWUtils {
namespace Names {
  
namespace ParNaming {
  /** Namespace for conventions on how to name parameters.
  **/

  // Map to get shorter naming convention for chiral config parameters
  static const std::map<std::string, std::string> chiral_naming_map = 
    {
      {PREW::GlobalVar::Chiral::eLpR, "LR"},
      {PREW::GlobalVar::Chiral::eRpL, "RL"},
      {PREW::GlobalVar::Chiral::eLpL, "LL"},
      {PREW::GlobalVar::Chiral::eRpR, "RR"}
    };
    
  std::string chi_xs_par_name ( 
    const std::string & distr_name,
    const std::string & chiral_config
  );
  
  std::string lumi_name ( int energy, std::string energy_unit="GeV" );
  
} // Namespace ParNaming
  
} // Namespace Names
} // Namespace PrEWUtils
  
#endif