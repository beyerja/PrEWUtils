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
      {PrEW::GlobalVar::Chiral::eLpR, "LR"},
      {PrEW::GlobalVar::Chiral::eRpL, "RL"},
      {PrEW::GlobalVar::Chiral::eLpL, "LL"},
      {PrEW::GlobalVar::Chiral::eRpR, "RR"}
    };
    
  std::string chi_xs_par_name ( 
    const std::string & distr_name,
    const std::string & chiral_config
  );
  
  std::string total_chi_xs_par_name ( const std::string & distr_name );
  std::string asymm_par_name ( 
    const std::string & distr_name,
    int asymm_index=0
  );
  
  std::string Af_par_name ( const std::string & distr_name );
  
  std::string lumi_name ( int energy, std::string energy_unit="GeV" );
  
} // Namespace ParNaming
  
} // Namespace Names
} // Namespace PrEWUtils
  
#endif