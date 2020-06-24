#ifndef LIB_COEFNAMING_H
#define LIB_COEFNAMING_H 1

// Includes from PrEW
#include "Data/DistrInfo.h"

#include <string>

namespace PrEWUtils {
namespace Names {
  
namespace CoefNaming {
  /** Namespace for conventions on how to name coefficients.
  **/

  std::string lumi_fraction_name ( 
    const PrEW::Data::DistrInfo & info_pol, 
    std::string energy_unit="GeV"
  );
  
  std::string chi_xs_coef_name ( const PrEW::Data::DistrInfo & info_chi );
  std::string chi_distr_coef_name ( const PrEW::Data::DistrInfo & info_chi, 
                                    const std::string type="signal" );
  
  std::string costheta_index_coef_name();
} // Namespace CoefNaming
  
} // Namespace Names
} // Namespace PrEWUtils

#endif