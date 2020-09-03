#include <DataHelp/DistrHelp.h>
#include <DataHelp/FitParHelp.h>
#include <Names/CoefNaming.h>
#include <Names/ParNaming.h>
#include <SetupHelp/RunInfo.h>

// includes from PrEW
#include <Data/FctLink.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

RunInfo::RunInfo(int energy) : m_energy(energy) {}

//------------------------------------------------------------------------------

void RunInfo::set_lumi(double val, double ini_unc) {
  /** Add luminosity.
   **/
  m_pars.push_back(
      PrEW::Fit::FitPar(Names::ParNaming::lumi_name(), val, ini_unc));
}

//------------------------------------------------------------------------------

void RunInfo::add_pol(const std::string &name, double val, double ini_unc) {
  /** Add a new single beam polarisation.
   **/
  m_pars.push_back(PrEW::Fit::FitPar(name, val, ini_unc));
}

//------------------------------------------------------------------------------

void RunInfo::add_pol_config(const std::string &config_name,
                             const std::string &e_pol_name,
                             const std::string &p_pol_name,
                             const std::string &e_pol_sign,
                             const std::string &p_pol_sign,
                             double lumi_fraction) {
  /** Add a polarisation configuration with a given luminosity fraction.
   **/
  // Add the instructions for the polarisation combination
  m_pol_links.push_back(PrEW::Data::PolLink(
      m_energy, config_name, e_pol_name, p_pol_name, e_pol_sign, p_pol_sign));

  // Add the instructions for the luminosity fraction
  std::string coef_name =
      Names::CoefNaming::lumi_fraction_name(config_name, m_energy);
  PrEW::Data::DistrInfo fixed_pol_info = {m_default_name, config_name,
                                          m_energy};

  PrEW::Data::FctLink fct_link{
      "LuminosityFraction", {Names::ParNaming::lumi_name()}, {coef_name}};
  m_pred_links.push_back(
      PrEW::Data::PredLink{fixed_pol_info, {fct_link}, {fct_link}});
  m_coefs.push_back(
      PrEW::Data::CoefDistr(coef_name, fixed_pol_info, lumi_fraction));
}

//------------------------------------------------------------------------------

void RunInfo::add_lumi_constr(double constr_val, double constr_unc) {
  /** Add a gaussian constraint on the luminosity.
   **/
  std::string name = Names::ParNaming::lumi_name();
  (this->find_par(name)).set_constrgauss(constr_val, constr_unc);
}

//------------------------------------------------------------------------------

void RunInfo::add_pol_constr(const std::string &name, double constr_val,
                             double constr_unc) {
  /** Add a gaussian constraint on the given single beam polarisation.
   **/
  (this->find_par(name)).set_constrgauss(constr_val, constr_unc);
}

//------------------------------------------------------------------------------

void RunInfo::fix_lumi() {
  /** Fix the luminosity parameter to its current value.
      Will not be varied by fit.
  **/
  std::string name = Names::ParNaming::lumi_name();
  (this->find_par(name)).fix();
}

//------------------------------------------------------------------------------

void RunInfo::fix_pol(const std::string &name) {
  /** Fix the polarisation parameter to its current value.
      Will not be varied by fit.
  **/
  (this->find_par(name)).fix();
}

//------------------------------------------------------------------------------
// Access functions

const PrEW::Fit::ParVec &RunInfo::get_pars() const { return m_pars; }
const PrEW::Data::PolLinkVec &RunInfo::get_pol_links() const {
  return m_pol_links;
}

//------------------------------------------------------------------------------

PrEW::Data::PredLinkVec
RunInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
  /** Return all predicition links relevant to the different polarisations
      configs of the given distributions.
   **/
  PrEW::Data::PredLinkVec pred_links{};
  // Replace the default name in the pred link info with the distribution names
  auto distr_names = DataHelp::DistrHelp::find_distr_names(infos);
  for (const auto &distr_name : distr_names) {
    for (const auto &pred_link : m_pred_links) {
      auto named_link = pred_link;
      named_link.m_info.m_distr_name = distr_name;
      pred_links.push_back(named_link);
    }
  }
  return pred_links;
}

//------------------------------------------------------------------------------

PrEW::Data::CoefDistrVec
RunInfo::get_coefs(const PrEW::Data::InfoVec &infos) const {
  /** Return coefficients for all given distributions.
   **/
  PrEW::Data::CoefDistrVec all_coefs{};

  // Here: All coefs are relevant for each distr
  auto distr_names = DataHelp::DistrHelp::find_distr_names(infos);
  for (const auto &distr_name : distr_names) {
    auto coefs = m_coefs;
    for (auto coef : coefs) {
      // Set the correct name in the coef info and add it to the coef list
      auto info = coef.get_info();
      info.m_distr_name = distr_name;
      coef.set_info(info);
      all_coefs.push_back(coef);
    }
  }

  return all_coefs;
}

//------------------------------------------------------------------------------

PrEW::Fit::FitPar &RunInfo::find_par(const std::string &name) {
  /** Find the parameter of given name in the parameter vector.
   **/
  return DataHelp::FitParHelp::find_par_in_vec(name, m_pars);
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils