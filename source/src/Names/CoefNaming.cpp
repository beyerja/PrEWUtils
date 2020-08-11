#include <Names/CoefNaming.h>

#include <string>

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
  const PrEW::Data::DistrInfo & info_chi
) {
  /** Naming convention for the coefficient that stores the total chiral cross
      section of a distribution.
  **/
  return "ChiXS_" + info_chi.m_distr_name + "_" + info_chi.m_pol_config +
         "_" + std::to_string(info_chi.m_energy);
}

//------------------------------------------------------------------------------

std::string 
CoefNaming::chi_distr_coef_name ( const PrEW::Data::DistrInfo & info_chi, 
                                  const std::string type ) {
  /** Naming convention for the coefficient that stores the differential 
      cross section as coefficients.
  **/
  return "ChiDistr_" + info_chi.m_distr_name + "_" + info_chi.m_pol_config +
         "_" + std::to_string(info_chi.m_energy) + "_" + type;
}

//------------------------------------------------------------------------------

std::string CoefNaming::costheta_index_coef_name(){
  /** Name for the coefficient that describes the index of the cos(Theta) 
      observable in the observables vector.
  **/
  return "CosThetaIndex";
}

//------------------------------------------------------------------------------

std::string CoefNaming::bin_width_coef_name(){
  /** Name for the coefficient that describes the width of the bins of the 
      distribution.
   **/
  return "BinWidth";
}

//------------------------------------------------------------------------------

} // Namespace Names
} // Namespace PrEWUtils