#ifndef LIB_MINIMIZERINFO_H
#define LIB_MINIMIZERINFO_H 1

#include "Minuit2/Minuit2Minimizer.h"

#include <vector>

namespace PrEWUtils {
namespace Names {

  struct MinimizerInfo {
    /** Struct that stores the information that defines a Minuit2 minimizer.
    **/
    
    ROOT::Minuit2::EMinimizerType m_type {};
    unsigned int m_max_fcn_calls {};
    unsigned int m_max_iters {};
    double m_tolerance {};
  };

  using MinInfoVec = std::vector<MinimizerInfo>;

} // Namespace PrEWUtils
} // Namespace Names

#endif