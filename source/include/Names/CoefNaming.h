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
    const PREW::Data::DistrInfo & info_pol, 
    std::string energy_unit="GeV"
  );
  
} // Namespace CoefNaming
  
} // Namespace Names
} // Namespace PrEWUtils

#endif