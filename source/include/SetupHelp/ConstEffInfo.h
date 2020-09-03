#ifndef LIB_CONSTEFFINFO_H
#define LIB_CONSTEFFINFO_H 1

// Includes from PrEW
#include <Data/DistrInfo.h>
#include <Data/PredLink.h>
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
  PrEW::Fit::FitPar m_eff_par;

public:
  // Constructors
  ConstEffInfo(const std::string &distr_name, double eff);

  // Modify
  void fix();
  void constrain(double val, double unc);

  // Access functions
  PrEW::Fit::ParVec get_pars() const;
  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
};

using ConstEffInfoVec = std::vector<ConstEffInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif