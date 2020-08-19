#ifndef LIB_CONSTEFFINFO_H
#define LIB_CONSTEFFINFO_H 1

// Includes from PrEW
#include <Data/FctLink.h>
#include <Fit/FitPar.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

class ConstEffInfo {
  /** Helper class that desribes the information connected to an constant 
      selection efficiency of a distribution.
  **/

  std::string m_distr_name{};
  double m_eff {};
  
  bool m_is_fixed {false};
  
public:
  // Constructors
  ConstEffInfo(const std::string &distr_name, double eff);

  // Modify 
  void fix();

  // Access functions
  const std::string &get_distr_name() const;
  double get_eff() const;

  // Functions needed for setting up PrEW fit
  PrEW::Fit::FitPar get_par() const;
  PrEW::Data::FctLink get_fct_link() const;
                                   
  // Operators
  bool operator==(const ConstEffInfo &other) const;
  bool operator==(const std::string &distr_name) const;

};

using ConstEffInfoVec = std::vector<ConstEffInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif