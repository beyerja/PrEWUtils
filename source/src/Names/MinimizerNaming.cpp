#include <Names/MinimizerNaming.h>

// Includes from PrEW
#include <CppUtils/Str.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace Names {

//------------------------------------------------------------------------------

MinInfoVec MinimizerNaming::read_mininimizer_str(const std::string & min_str) {
  /** Interpret the minimizer instruction string.
      Instruction must be of from 
      "MinimizerID1(MaxFcnCalls,MaxIters,Tolerance)->MinimizerID1(MaxFcnCalls,MaxIters,Tolerance)->..."
      where the bracket option is optional. 
  **/
  MinInfoVec min_info_vec {};
  
  // Default values
  unsigned int default_max_fcn_calls = 100000;
  unsigned int default_max_iters = 100000;
  double default_tolerance = 0.0001;
  
  auto minimizers_split = 
      PREW::CppUtils::Str::string_to_vec( min_str, "->" );
  
  
  for (const auto & individual_min: minimizers_split) {
    auto open_bracket_split = 
      PREW::CppUtils::Str::string_to_vec( individual_min, "(" );
    
    // Finding the type of minimizer
    auto min_typename = open_bracket_split.at(0);
    
    // New minimizer information
    MinimizerInfo min_info {
      Names::MinimizerNaming::minimizer_naming_map.at(min_typename),
      default_max_fcn_calls,
      default_max_iters,
      default_tolerance
    };
    
    if (open_bracket_split.size() == 2) {
      // Bracket options are given, use them
      auto comma_split = 
        PREW::CppUtils::Str::string_to_vec( open_bracket_split.at(1), "," );
      
      if ( comma_split.size() != 3 ) {
        spdlog::warn("MinimizerNaming: Faulty Minimizer options {}", individual_min);
        continue;
      }
      
      // Remove closing bracket from end of last string
      auto tolerance_str = 
        PREW::CppUtils::Str::string_to_vec( comma_split.at(2), ")" ).at(0);
      
      min_info.m_max_fcn_calls = std::stoi(comma_split.at(0));
      min_info.m_max_iters = std::stoi(comma_split.at(1));
      min_info.m_tolerance = std::stod(tolerance_str);
    }
    
    min_info_vec.push_back(min_info);
  }
  
  return min_info_vec;
}

//------------------------------------------------------------------------------

} // Namespace Names
} // Namespace PrEWUtils