#ifndef LIB_DIFERMIONPARAMINFO_H
#define LIB_DIFERMIONPARAMINFO_H 1

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

class DifermionParamInfo {
  std::string m_distr_name{};
  int m_costheta_index{};
  PrEW::Fit::ParVec m_pars {};
  PrEW::Data::PredLinkVec m_pred_links{};

public:
  // Constructors
  DifermionParamInfo(const std::string &distr_name, int costheta_index = 0);

  // Access functions
  const PrEW::Fit::ParVec & get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec
  get_coefs(const PrEW::Data::PredDistrVec &preds) const;

protected:
  PrEW::Data::FctLink get_fct_link(const PrEW::Data::DistrInfo &info) const;
};

using DifermionParamInfoVec = std::vector<DifermionParamInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif