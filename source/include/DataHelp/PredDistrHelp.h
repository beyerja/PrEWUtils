#ifndef LIB_PREDDISTRHELP_H
#define LIB_PREDDISTRHELP_H 1

// Includes from PrEW
#include "Data/PredDistr.h"

#include <string>

namespace PrEWUtils {
namespace DataHelp {
  
namespace PredDistrHelp {
  /** Namespace for helper functions relating to the PrEW PredDistr class.
  **/
  
  double get_pred_sum (
    const PREW::Data::PredDistr & pred, 
    const std::string & type
  );
  
}
  
}  
}

#endif