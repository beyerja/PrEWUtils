#include <Names/CoefNaming.h>

namespace PrEWUtils {
namespace Names {

//------------------------------------------------------------------------------

std::string CoefNaming::lumi_fraction_name ( 
  const PREW::Data::DistrInfo & info_pol, 
  std::string energy_unit
) {
  /** Convention for naming the luminosity fraction coefficient.
  **/
  return  "LumiFr" + info_pol.m_pol_config 
          + std::to_string(info_pol.m_energy) + energy_unit;
}

//------------------------------------------------------------------------------

} // Namespace Names
} // Namespace PrEWUtils