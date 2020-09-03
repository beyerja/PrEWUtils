#include <DataHelp/PredDistrHelp.h>
#include <Names/CoefNaming.h>
#include <Names/FctNaming.h>
#include <Names/ParNaming.h>
#include <SetupHelp/AfInfo.h>

// inputs from PrEW
#include <Data/DistrUtils.h>

// Standard library
#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

AfInfo::AfInfo(const std::string &distr_name, const std::string &Af_name,
               int costheta_index)
    : m_distr_name(distr_name), m_costheta_index(costheta_index),
      m_par(PrEW::Fit::FitPar(Names::ParNaming::Af_par_name(distr_name), 0,
                              0.0001)) {
  /** If the asymmetry parameter name is "default", then the PrEWUtils
      convention is used.
   **/

  // Set up the parameter
  if (Af_name != "default") {
    m_par =
        PrEW::Fit::FitPar(Af_name, m_par.get_val_ini(), m_par.get_unc_ini());
  }

  // Create the prediction link (energy to be filled later)
  PrEW::Data::DistrInfo info_LR{distr_name, PrEW::GlobalVar::Chiral::eLpR, 0};
  PrEW::Data::DistrInfo info_RL{distr_name, PrEW::GlobalVar::Chiral::eRpL, 0};

  auto fct_link_LR = this->get_fct_link(info_LR);
  auto fct_link_RL = this->get_fct_link(info_RL);

  m_pred_links.push_back(PrEW::Data::PredLink{info_LR, {fct_link_LR}, {}});
  m_pred_links.push_back(PrEW::Data::PredLink{info_RL, {fct_link_RL}, {}});
}

//------------------------------------------------------------------------------
// Access functions

// TODO in pred link replace energy
// TODO in coef replace energy and set values

PrEW::Fit::ParVec AfInfo::get_pars() const { return {m_par}; }

PrEW::Data::PredLinkVec
AfInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
  /** Get the linking instructions for PrEW.
   **/
  PrEW::Data::PredLinkVec pred_links = m_pred_links;

  // Set the correct energy
  int energy = infos.at(0).m_energy;
  for (auto &pred_link : pred_links) {
    pred_link.m_info.m_energy = energy;
  }

  return pred_links;
}

PrEW::Data::CoefDistrVec
AfInfo::get_coefs(const PrEW::Data::PredDistrVec &preds) const {
  /** Get all the coefficients needed for the function related to the chiral
      cross sections.
   **/
  int energy = preds.at(0).get_info().m_energy;
  PrEW::Data::DistrInfo info_LR{m_distr_name, PrEW::GlobalVar::Chiral::eLpR,
                                energy};
  PrEW::Data::DistrInfo info_RL{m_distr_name, PrEW::GlobalVar::Chiral::eRpL,
                                energy};

  PrEW::Data::CoefDistrVec coefs{}; // output vector

  // Find the relevant distributions
  auto pred_LR = PrEW::Data::DistrUtils::subvec_info(preds, info_LR).at(0);
  auto pred_RL = PrEW::Data::DistrUtils::subvec_info(preds, info_RL).at(0);

  // Find the distribution sums and differential values
  auto LR_sum = DataHelp::PredDistrHelp::get_pred_sum(pred_LR, "signal");
  auto RL_sum = DataHelp::PredDistrHelp::get_pred_sum(pred_RL, "signal");
  auto LR_diff = DataHelp::PredDistrHelp::pred_to_coef(pred_LR, "signal");
  auto RL_diff = DataHelp::PredDistrHelp::pred_to_coef(pred_RL, "signal");

  // Differential distribution only need for itself
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_distr_coef_name(info_LR), info_LR, LR_diff));
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_distr_coef_name(info_RL), info_RL, RL_diff));

  // Sum coefficients are needed for both
  auto diff_coef_name_LR = Names::CoefNaming::chi_xs_coef_name(info_LR);
  auto diff_coef_name_RL = Names::CoefNaming::chi_xs_coef_name(info_RL);
  for (auto info : {info_LR, info_RL}) {
    coefs.push_back(PrEW::Data::CoefDistr(diff_coef_name_LR, info, LR_sum));
    coefs.push_back(PrEW::Data::CoefDistr(diff_coef_name_RL, info, RL_sum));
  }

  // Add index coefficients
  coefs.push_back(
      PrEW::Data::CoefDistr(Names::CoefNaming::costheta_index_coef_name(),
                            info_LR, m_costheta_index));
  coefs.push_back(
      PrEW::Data::CoefDistr(Names::CoefNaming::costheta_index_coef_name(),
                            info_RL, m_costheta_index));

  return coefs;
}

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

PrEW::Data::FctLink
AfInfo::get_fct_link(const PrEW::Data::DistrInfo &info) const {
  /** Get PrEW function link for a single chiral configuration.
      Chiral config must use PrEW conventions or naming chiral configs.
      These function links are used by PrEW as instructions for how to set
      up the bin predictions.
   **/
  PrEW::Data::FctLink fct_link{};
  fct_link.m_pars = {m_par.get_name()};
  fct_link.m_coefs = {Names::CoefNaming::chi_xs_coef_name(info),
                      Names::CoefNaming::chi_distr_coef_name(info, "signal"),
                      Names::CoefNaming::costheta_index_coef_name()};

  if (info.m_pol_config == PrEW::GlobalVar::Chiral::eLpR) {
    fct_link.m_fct_name = "AsymmFactorLR_Af_2f";
  } else if (info.m_pol_config == PrEW::GlobalVar::Chiral::eRpL) {
    fct_link.m_fct_name = "AsymmFactorRL_Af_2f";
  } else {
    throw std::invalid_argument("AfInfo: Need PrEW eLpR/eRpL chiral configs!");
  }

  return fct_link;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils