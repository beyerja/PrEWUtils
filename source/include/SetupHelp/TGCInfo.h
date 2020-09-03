#ifndef LIB_TGCINFO_H
#define LIB_TGCINFO_H 1

// Includes from PrEW
#include <Data/CoefDistr.h>
#include <Data/PredLink.h>
#include <Fit/FitPar.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

class TGCInfo {
  /** Helper class that desribes the information connected to the TGC
      parametrisation.
  **/

  std::string m_mode{};
  std::vector<std::string> m_distrs{};
  
  PrEW::Fit::ParVec m_pars{};
  PrEW::Data::CoefDistrVec m_coefs{};
  PrEW::Data::PredLinkVec m_pred_links{};

public:
  // Constructors
  TGCInfo(const std::vector<std::string> &distrs,
          const std::string &mode = "linear");

  // Access functions
  const PrEW::Fit::ParVec &get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec get_coefs(const PrEW::Data::InfoVec &infos) const;
  
protected:
  bool affects_distr(const PrEW::Data::DistrInfo &info) const;
};

using TGCInfoVec = std::vector<TGCInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils


#endif