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
                                       const DifermionPars &par_info)
    : m_distr_name(distr_name), m_par_info(par_info) {

  // Helper function to deal with default values of parameter names
  auto p_name = [distr_name](const std::string &par, const std::string &name) {
    if (name == DifermionPars::def_name) {
      return par + "_" + distr_name;
    } else {
      return name;
    }
  };

  using FitPar = PrEW::Fit::FitPar;
  auto s0 = FitPar(p_name("s0", m_par_info.s0_name), m_par_info.s0_val, 0.001);
  auto Ae = FitPar(p_name("Ae", m_par_info.Ae_name), m_par_info.Ae_val, 0.001);
  auto Af = FitPar(p_name("Af", m_par_info.Af_name), m_par_info.Af_val, 0.001);
  auto ef = FitPar(p_name("ef", m_par_info.ef_name), m_par_info.ef_val, 0.001);
  auto kL = FitPar(p_name("kL", m_par_info.kL_name), m_par_info.kL_val, 0.001);
  auto kR = FitPar(p_name("kR", m_par_info.kR_name), m_par_info.kR_val, 0.001);

  // clang-format off
  if (m_par_info.s0_constr != DifermionPars::ParConstr()) { 
    auto c = m_par_info.s0_constr; s0.set_constrgauss(c.first, c.second); }
  if (m_par_info.Ae_constr != DifermionPars::ParConstr()) { 
    auto c = m_par_info.Ae_constr; Ae.set_constrgauss(c.first, c.second); }
  if (m_par_info.Af_constr != DifermionPars::ParConstr()) { 
    auto c = m_par_info.Af_constr; Af.set_constrgauss(c.first, c.second); }
  if (m_par_info.ef_constr != DifermionPars::ParConstr()) { 
    auto c = m_par_info.ef_constr; ef.set_constrgauss(c.first, c.second); }
  if (m_par_info.kL_constr != DifermionPars::ParConstr()) { 
    auto c = m_par_info.kL_constr; kL.set_constrgauss(c.first, c.second); }
  if (m_par_info.kR_constr != DifermionPars::ParConstr()) { 
    auto c = m_par_info.kR_constr; kR.set_constrgauss(c.first, c.second); }
  // clang-format on

  m_pars = {s0, Ae, Af, ef, kL, kR};

  // Create the prediction link (correct energy to be filled later)
  PrEW::Data::DistrInfo info_LR = this->get_LR_info(0);
  PrEW::Data::DistrInfo info_RL = this->get_RL_info(0);

  auto fct_link_LR = this->get_fct_link(info_LR);
  auto fct_link_RL = this->get_fct_link(info_RL);

  m_pred_links.push_back(PrEW::Data::PredLink{info_LR, {fct_link_LR}, {}});
  m_pred_links.push_back(PrEW::Data::PredLink{info_RL, {fct_link_RL}, {}});
}

//------------------------------------------------------------------------------
// Access functions

const PrEW::Fit::ParVec &DifermionParamInfo::get_pars() const { return m_pars; }

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
  PrEW::Data::DistrInfo info_LR = this->get_LR_info(energy);
  PrEW::Data::DistrInfo info_RL = this->get_RL_info(energy);

  PrEW::Data::CoefDistrVec coefs{}; // output vector

  // Find the relevant distributions
  auto pred_LR = PrEW::Data::DistrUtils::subvec_info(preds, info_LR).at(0);
  auto pred_RL = PrEW::Data::DistrUtils::subvec_info(preds, info_RL).at(0);

  // Find the distribution differential values
  // -> Each chiral distribution only needs itself as coefs
  auto LR_diff = DataHelp::PredDistrHelp::pred_to_coef(pred_LR, "signal");
  auto RL_diff = DataHelp::PredDistrHelp::pred_to_coef(pred_RL, "signal");
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_distr_coef_name(info_LR), info_LR, LR_diff));
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_distr_coef_name(info_RL), info_RL, RL_diff));

  // Find total chiral cross sections
  // -> both needed by both
  auto LR_sum = DataHelp::PredDistrHelp::get_pred_sum(pred_LR, "signal");
  auto RL_sum = DataHelp::PredDistrHelp::get_pred_sum(pred_RL, "signal");
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_xs_coef_name(info_LR), info_LR, LR_sum));
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_xs_coef_name(info_RL), info_LR, RL_sum));
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_xs_coef_name(info_LR), info_RL, LR_sum));
  coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::chi_xs_coef_name(info_RL), info_RL, RL_sum));

  // Add index coefficients
  coefs.push_back(
      PrEW::Data::CoefDistr(Names::CoefNaming::costheta_index_coef_name(),
                            info_LR, m_par_info._costheta_index));
  coefs.push_back(
      PrEW::Data::CoefDistr(Names::CoefNaming::costheta_index_coef_name(),
                            info_RL, m_par_info._costheta_index));

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
  for (const auto &par : m_pars) {
    fct_link.m_pars.push_back(par.get_name());
  }
  fct_link.m_coefs = {
      Names::CoefNaming::chi_distr_coef_name(info),
      Names::CoefNaming::chi_xs_coef_name(this->get_LR_info(info.m_energy)),
      Names::CoefNaming::chi_xs_coef_name(this->get_RL_info(info.m_energy)),
      Names::CoefNaming::costheta_index_coef_name()};

  if (info.m_pol_config == PrEW::GlobalVar::Chiral::eLpR) {
    fct_link.m_fct_name = "General2fParam_LR";
  } else if (info.m_pol_config == PrEW::GlobalVar::Chiral::eRpL) {
    fct_link.m_fct_name = "General2fParam_RL";
  } else {
    throw std::invalid_argument(
        "DifermionParamInfo: Need PrEW eLpR/eRpL chiral configs!");
  }

  return fct_link;
}

PrEW::Data::DistrInfo DifermionParamInfo::get_LR_info(int energy) const {
  return {m_distr_name, PrEW::GlobalVar::Chiral::eLpR, energy};
}

PrEW::Data::DistrInfo DifermionParamInfo::get_RL_info(int energy) const {
  return {m_distr_name, PrEW::GlobalVar::Chiral::eRpL, energy};
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils