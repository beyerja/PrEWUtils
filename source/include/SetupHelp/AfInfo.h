#ifndef LIB_AFINFO_H
#define LIB_AFINFO_H 1

// Includes from PrEW
#include <Data/CoefDistr.h>
#include <Data/FctLink.h>
#include <Data/PredDistr.h>
#include <Data/PredLink.h>
#include <Fit/FitPar.h>
#include <GlobalVar/Chiral.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

class AfInfo {
  /** Helper class that describes the information connected to an Af
      parametrisation for a 2-fermion distribution.
      Af = [ (c_L^f)^2 - (c_R^f)^2 ] / [ (c_L^f)^2 + (c_R^f)^2 ]
      WARNING:
        Replaces the original distribution with the SM tree level
        expectation (and does not take into account bin width)!
  **/

  std::string m_distr_name{};
  int m_costheta_index{};
  PrEW::Fit::FitPar m_par;
  PrEW::Data::PredLinkVec m_pred_links{};

public:
  // Constructors
  AfInfo(const std::string &distr_name, const std::string &Af_name = "default",
         int costheta_index = 0);

  // Access functions
  PrEW::Fit::ParVec get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec
  get_coefs(const PrEW::Data::PredDistrVec &preds) const;

protected:
  PrEW::Data::FctLink get_fct_link(const PrEW::Data::DistrInfo &info) const;
};

using AfInfoVec = std::vector<AfInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif