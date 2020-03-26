#ifndef LIB_MINIMIZERNAMING_H
#define LIB_MINIMIZERNAMING_H 1

// Includes from ROOT
#include "Minuit2/Minuit2Minimizer.h"

#include <string>
#include <map>

namespace PrEWUtils {
namespace Names {
  
namespace MinimizerNaming {
  /** Namespace for conventions on how to name Minuit2 minimizers.
  **/

  static const std::map<std::string, ROOT::Minuit2::EMinimizerType> 
    minimizer_naming_map = 
    {
      {"Migrad",      ROOT::Minuit2::kMigrad},
      {"Simplex",     ROOT::Minuit2::kSimplex},
      {"Combined",    ROOT::Minuit2::kCombined},
      {"Scan",        ROOT::Minuit2::kScan},
      {"Fumili",      ROOT::Minuit2::kFumili},
      {"MigradBFGS",  ROOT::Minuit2::kMigradBFGS},
    };
  
} // Namespace MinimizerNaming
  
} // Namespace Names
} // Namespace PrEWUtils
  
#endif