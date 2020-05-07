#include <Names/CoefNaming.h>

namespace PrEWUtils {
namespace Names {

//------------------------------------------------------------------------------

std::string CoefNaming::lumi_fraction_name ( 
  const PrEW::Data::DistrInfo & info_pol, 
  std::string energy_unit
) {
  /** Convention for naming the luminosity fraction coefficient.
  **/
  return  "LumiFr" + info_pol.m_pol_config 
          + std::to_string(info_pol.m_energy) + energy_unit;
}

//------------------------------------------------------------------------------

std::string CoefNaming::chi_xs_coef_name ( 
  const std::string & distr_name,
  const std::string & chiral_config
) {
  /** Naming convention for the coefficient that stores the total chiral cross
      section of a distribution.
  **/
  return "ChiXS_" + distr_name + "_" + chiral_config;
}

//------------------------------------------------------------------------------

} // Namespace Names
} // Namespace PrEWUtils