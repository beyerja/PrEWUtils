#include <Names/CoefNaming.h>
#include <Names/FctNaming.h>
#include <Names/ParNaming.h>
#include <SetupHelp/AfInfo.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

AfInfo::AfInfo(const std::string &distr_name, const std::string &Af_name,
               const std::string &LR_config, const std::string &RL_config)
    : m_distr_name(distr_name), m_Af_name(Af_name), m_LR_config(LR_config),
      m_RL_config(RL_config) {
  /** Constructor copies given parameters.
      If the asymmetry parameter name is "default", then the PrEWUtils
   convention is used.
   **/

  if (m_Af_name == "default") {
    m_Af_name = Names::ParNaming::Af_par_name(m_distr_name);
  }
}

//------------------------------------------------------------------------------
// Access functions

const std::string &AfInfo::get_distr_name() const { return m_distr_name; }
const std::string &AfInfo::get_Af_name() const { return m_Af_name; }
const std::string &AfInfo::get_LR_config() const { return m_LR_config; }
const std::string &AfInfo::get_RL_config() const { return m_RL_config; }

//------------------------------------------------------------------------------
// Functions to use asymmetry in PrEW

PrEW::Fit::FitPar AfInfo::get_par(double initial_val,
                                  double initial_unc) const {
  /** Get the PrEW FitPar for this Af parameter.
   **/
  return PrEW::Fit::FitPar(m_Af_name, initial_val, initial_unc);
}

PrEW::Data::FctLink
AfInfo::get_fct_link(int energy, const std::string &chiral_config) const {
  /** Get PrEW function link for a single chiral configuration.
      Chiral config must use PrEW conventions or naming chiral configs.
      These function links are used by PrEW as instructions for how to set
   up the bin predictions.
   **/
  PrEW::Data::DistrInfo info{m_distr_name, chiral_config, energy};

  PrEW::Data::FctLink fct_link{};
  fct_link.m_pars = {m_Af_name};
  fct_link.m_coefs = {Names::CoefNaming::chi_xs_coef_name(info),
                      Names::CoefNaming::chi_distr_coef_name(info, "signal"),
                      Names::CoefNaming::costheta_index_coef_name()};

  if (chiral_config == PrEW::GlobalVar::Chiral::eLpR) {
    fct_link.m_fct_name = "AsymmFactorLR_Af_2f";
  } else if (chiral_config == PrEW::GlobalVar::Chiral::eRpL) {
    fct_link.m_fct_name = "AsymmFactorRL_Af_2f";
  } else {
    throw std::invalid_argument("AfInfo: Need PrEW eLpR/eRpL chiral configs!");
  }

  return fct_link;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils