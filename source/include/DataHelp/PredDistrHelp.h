#ifndef LIB_PREDDISTRHELP_H
#define LIB_PREDDISTRHELP_H 1

// Includes from PrEW
#include <Data/CoefDistr.h>
#include "Data/PredDistr.h"

#include <string>

namespace PrEWUtils {
namespace DataHelp {
  
namespace PredDistrHelp {
  /** Namespace for helper functions relating to the PrEW PredDistr class.
  **/
  
  double get_pred_sum (
    const PrEW::Data::PredDistr & pred, 
    const std::string & type
  );
  
  std::vector<double> pred_to_coef(const PrEW::Data::PredDistr &pred,
                                   const std::string &type = "signal");
  
}
  
}  
}

#endif