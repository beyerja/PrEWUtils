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
                                       const DifermionPars &par_info,
                                       bool polarised)
    : m_distr_name(distr_name), m_par_info(par_info), m_polarised(polarised) {
  if (m_polarised) {
    this->create_polarised_pars();
  } else {
    this->create_unpolarised_pars();
  }

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

std::string DifermionParamInfo::p_name(const std::string &par,
                                       const std::string &name) const {
  /** Helper function to deal with default values of parameter names.
   **/
  if (name == DifermionPars::def_name) {
    return par + "_" + m_distr_name;
  } else {
    return name;
  }
}

void DifermionParamInfo::create_polarised_pars() {
  /** Create the fit parameters which are necessary for the polarised scenario.
   **/

  using FitPar = PrEW::Fit::FitPar;
  const auto &pinfo = m_par_info;

  auto s0 = FitPar(this->p_name("s0", pinfo.s0_name), pinfo.s0_val, 0.001);
  auto Ae = FitPar(this->p_name("Ae", pinfo.Ae_name), pinfo.Ae_val, 0.001);
  auto Af = FitPar(this->p_name("Af", pinfo.Af_name), pinfo.Af_val, 0.001);
  auto ef = FitPar(this->p_name("ef", pinfo.ef_name), pinfo.ef_val, 0.001);
  auto k0 = FitPar(this->p_name("k0", pinfo.k0_name), pinfo.k0_val, 0.001);
  auto dk = FitPar(this->p_name("dk", pinfo.dk_name), pinfo.dk_val, 0.001);

  // clang-format off
  if (pinfo.s0_constr != DifermionPars::ParConstr()) { 
   auto c = pinfo.s0_constr; s0.set_constrgauss(c.first, c.second); }
  if (pinfo.Ae_constr != DifermionPars::ParConstr()) { 
   auto c = pinfo.Ae_constr; Ae.set_constrgauss(c.first, c.second); }
  if (pinfo.Af_constr != DifermionPars::ParConstr()) { 
   auto c = pinfo.Af_constr; Af.set_constrgauss(c.first, c.second); }
  if (pinfo.ef_constr != DifermionPars::ParConstr()) { 
   auto c = pinfo.ef_constr; ef.set_constrgauss(c.first, c.second); }
  if (pinfo.k0_constr != DifermionPars::ParConstr()) { 
   auto c = pinfo.k0_constr; k0.set_constrgauss(c.first, c.second); }
  if (pinfo.dk_constr != DifermionPars::ParConstr()) { 
   auto c = pinfo.dk_constr; dk.set_constrgauss(c.first, c.second); }
   
  if (pinfo.s0_fixed) { s0.fix(); }
  if (pinfo.Ae_fixed) { Ae.fix(); }
  if (pinfo.Af_fixed) { Af.fix(); }
  if (pinfo.ef_fixed) { ef.fix(); }
  if (pinfo.k0_fixed) { k0.fix(); }
  if (pinfo.dk_fixed) { dk.fix(); }
  // clang-format on

  m_pars = {s0, Ae, Af, ef, k0, dk};
}

void DifermionParamInfo::create_unpolarised_pars() {
  /** Create the fit parameters which are necessary for the unpolarised
   *scenario.
   **/
  using FitPar = PrEW::Fit::FitPar;
  const auto &pinfo = m_par_info;

  auto AFB_val = 3. / 8. * (pinfo.ef_val + 2. * pinfo.Ae_val * pinfo.Af_val);

  auto s0 = FitPar(this->p_name("s0", pinfo.s0_name), pinfo.s0_val, 0.001);
  auto Afb = FitPar(this->p_name("AFB", pinfo.ef_name), AFB_val, 0.001);
  auto k0 = FitPar(this->p_name("k0", pinfo.k0_name), pinfo.k0_val, 0.001);

  // clang-format off
  if (pinfo.s0_constr != DifermionPars::ParConstr()) { 
    auto c = pinfo.s0_constr; s0.set_constrgauss(c.first, c.second); }
  if (pinfo.ef_constr != DifermionPars::ParConstr()) { 
    auto c = pinfo.ef_constr; Afb.set_constrgauss(c.first, c.second); }
  if (pinfo.k0_constr != DifermionPars::ParConstr()) { 
    auto c = pinfo.k0_constr; k0.set_constrgauss(c.first, c.second); }

  if (pinfo.s0_fixed) { s0.fix(); }
  if (pinfo.ef_fixed) { Afb.fix(); }
  if (pinfo.k0_fixed) { k0.fix(); }
  // clang-format on

  m_pars = {s0, Afb, k0};
}

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

  if (!m_polarised) {
    fct_link.m_fct_name = "Unpol2fParam";
  } else if (info.m_pol_config == PrEW::GlobalVar::Chiral::eLpR) {
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