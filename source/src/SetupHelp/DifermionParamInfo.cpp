#include <DataHelp/PredDistrHelp.h>
#include <Names/CoefNaming.h>
#include <Names/FctNaming.h>
#include <Names/ParNaming.h>
#include <SetupHelp/DifermionParamInfo.h>

// inputs from PrEW
#include <Data/DistrUtils.h>

// Standard library
#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

DifermionParamInfo::DifermionParamInfo(const std::string &distr_name,
               int costheta_index)
    : m_distr_name(distr_name), m_costheta_index(costheta_index) {
      
      
   m_pars = {
     PrEW::Fit::FitPar("Sigma0", 2500., 1.),
     PrEW::Fit::FitPar("Ae", 0.21, 0.0001),
     PrEW::Fit::FitPar("Af", 0.20, 0.0001),
     PrEW::Fit::FitPar("AR", 0.25, 0.0001),
     PrEW::Fit::FitPar("kL", 0.02, 0.0001),
     PrEW::Fit::FitPar("kR", 0.02, 0.0001)
   };
   
  // m_pars[0].fix();
  // m_pars[1].fix();
  // m_pars[2].fix();
  // m_pars[3].fix();
  // m_pars[4].fix();
  // m_pars[5].fix();

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

const PrEW::Fit::ParVec & DifermionParamInfo::get_pars() const { return m_pars; }

PrEW::Data::PredLinkVec
DifermionParamInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
  /** Get the linking instructions for PrEW.
   **/
  spdlog::debug("DifermionParamInfo: Getting prediction links.");
  PrEW::Data::PredLinkVec pred_links = m_pred_links;

  // Set the correct energy
  int energy = infos.at(0).m_energy;
  for (auto &pred_link : pred_links) {
    pred_link.m_info.m_energy = energy;
  }

  return pred_links;
}

PrEW::Data::CoefDistrVec
DifermionParamInfo::get_coefs(const PrEW::Data::PredDistrVec &preds) const {
  /** Get all the coefficients needed for the function related to the chiral
      cross sections.
   **/
  spdlog::debug("DifermionParamInfo: Getting coefficients.");
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
  auto LR_diff = DataHelp::PredDistrHelp::pred_to_coef(pred_LR, "signal");
  auto RL_diff = DataHelp::PredDistrHelp::pred_to_coef(pred_RL, "signal");

  // Differential distribution only need for itself
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_distr_coef_name(info_LR), info_LR, LR_diff));
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_distr_coef_name(info_RL), info_RL, RL_diff));

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
DifermionParamInfo::get_fct_link(const PrEW::Data::DistrInfo &info) const {
  /** Get PrEW function link for a single chiral configuration.
      Chiral config must use PrEW conventions or naming chiral configs.
      These function links are used by PrEW as instructions for how to set
      up the bin predictions.
   **/
  PrEW::Data::FctLink fct_link{};
  for (const auto & par: m_pars) {fct_link.m_pars.push_back(par.get_name());}
  fct_link.m_coefs = {Names::CoefNaming::chi_distr_coef_name(info, "signal"),
                      Names::CoefNaming::costheta_index_coef_name()};

  if (info.m_pol_config == PrEW::GlobalVar::Chiral::eLpR) {
    fct_link.m_fct_name = "General2fParam_LR";
  } else if (info.m_pol_config == PrEW::GlobalVar::Chiral::eRpL) {
    fct_link.m_fct_name = "General2fParam_RL";
  } else {
    throw std::invalid_argument("DifermionParamInfo: Need PrEW eLpR/eRpL chiral configs!");
  }

  return fct_link;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils