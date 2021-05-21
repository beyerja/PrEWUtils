#include <DataHelp/CoefDistrHelp.h>
#include <DataHelp/DistrHelp.h>
#include <DataHelp/FitParHelp.h>
#include <DataHelp/PredLinkHelp.h>
#include <Setups/FitModifier.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace Setups {

//------------------------------------------------------------------------------

FitModifier::FitModifier(int energy) : m_energy(energy) {}

//------------------------------------------------------------------------------
// Access functions

int FitModifier::get_energy() const { return m_energy; }

//------------------------------------------------------------------------------
// Modification instructions
//------------------------------------------------------------------------------

void FitModifier::add(SetupHelp::AfInfo info) { m_Af_infos.push_back(info); }
void FitModifier::add(SetupHelp::DifermionParamInfo info) { m_difermion_param_infos.push_back(info); }

//------------------------------------------------------------------------------

void FitModifier::set_par_ordering(
    const SetupHelp::ParOrder::Ordering &ordering,
    const SetupHelp::ParOrder::IDMap &id_map) {
  /** Specify an ordering for the output parameters
   **/
  m_par_ordering = ordering;
  m_par_id_map = id_map;
}

//------------------------------------------------------------------------------
// Modification

void FitModifier::modify_setup(PrEW::Connect::DataConnector *connector,
                               PrEW::Fit::ParVec *pars) const {
  /** Modify the given setup according to the rules provided previously.
   **/
  spdlog::debug("Applying modifications.");
  this->apply_Af_mod(connector, pars);
  this->apply_2f_mod(connector, pars);

  spdlog::debug("Reordering parameters.");
  this->order_pars(pars);

  this->print_result(connector, pars);
}

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

void FitModifier::apply_Af_mod(PrEW::Connect::DataConnector *connector,
                               PrEW::Fit::ParVec *pars) const {
  /** Modify the setup with new Af info.
   **/
  auto infos = DataHelp::DistrHelp::find_infos(connector->get_pred_distrs());
  for (const auto &Af_info : m_Af_infos) {
    // Add new parameters
    DataHelp::FitParHelp::add_pars_to_vec(Af_info.get_pars(), *pars);

    // Add new connector info
    auto new_coefs = Af_info.get_coefs(connector->get_pred_distrs());
    auto new_pred_links = Af_info.get_pred_links(infos);
    this->add_to_connector(connector, new_coefs, new_pred_links);
  }
}

void FitModifier::apply_2f_mod(PrEW::Connect::DataConnector *connector,
                               PrEW::Fit::ParVec *pars) const {
  /** Modify the setup with new Af info.
   **/
  auto infos = DataHelp::DistrHelp::find_infos(connector->get_pred_distrs());
  for (const auto &difermion_param_info : m_difermion_param_infos) {
    // Add new parameters
    DataHelp::FitParHelp::add_pars_to_vec(difermion_param_info.get_pars(), *pars);

    // Add new connector info
    auto new_coefs = difermion_param_info.get_coefs(connector->get_pred_distrs());
    auto new_pred_links = difermion_param_info.get_pred_links(infos);
    this->add_to_connector(connector, new_coefs, new_pred_links);
  }
}

//------------------------------------------------------------------------------

void FitModifier::add_to_connector(
    PrEW::Connect::DataConnector *connector,
    const PrEW::Data::CoefDistrVec &coef_distrs,
    const PrEW::Data::PredLinkVec &pred_links) const {
  /** Replace the connector by one with the given distributions.
   **/
  // Prediction and pol links can't be modified right now
  auto preds_mod = connector->get_pred_distrs();
  auto pol_links_mod = connector->get_pol_links();

  // Add the new coefs and prediction links
  auto coefs_mod = connector->get_coef_distrs();
  auto pred_links_mod = connector->get_pred_links();
  DataHelp::CoefDistrHelp::add_coefs_to_vec(coef_distrs, coefs_mod);
  DataHelp::PredLinkHelp::add_links_to_vec(pred_links, pred_links_mod);

  // Replace the connector
  *connector = PrEW::Connect::DataConnector(preds_mod, coefs_mod,
                                            pred_links_mod, pol_links_mod);
}

//------------------------------------------------------------------------------

void FitModifier::order_pars(PrEW::Fit::ParVec *pars) const {
  /** Order the parameters according to the specification.
   **/
  *pars =
      SetupHelp::ParOrder::reorder_pars(*pars, m_par_ordering, m_par_id_map);
}

//------------------------------------------------------------------------------

void FitModifier::print_result(PrEW::Connect::DataConnector *connector,
                               PrEW::Fit::ParVec *pars) const {
  /** Print the result of the complete setup for debugging.
   **/
  spdlog::debug("FitModifier: Printing results of completed setup");
  spdlog::debug("Modified Parameters:");
  std::string par_str{" "};
  for (const auto &par : *pars) {
    par_str += " " + par.get_name() + " |";
  }
  spdlog::debug(par_str);

  spdlog::debug("Modified coefficients:");
  for (const auto &used_coef : connector->get_coef_distrs()) {
    const auto &info = used_coef.get_info();
    spdlog::debug("{} for {} @ {} & {}", used_coef.get_coef_name(),
                  info.m_distr_name, info.m_energy, info.m_pol_config);
    spdlog::debug(" -> First coef value: {}", used_coef.get_coef(0));
  }
  spdlog::debug("Modified prediction links:");
  for (const auto &pred_link : connector->get_pred_links()) {
    const auto &info = pred_link.get_info();
    spdlog::debug("For distribution {} @ {} & {}", info.m_distr_name,
                  info.m_energy, info.m_pol_config);
    std::string sig_fcts{" "}, bkg_fcts{" "};
    for (const auto &sig_fct : pred_link.m_fcts_links_sig) {
      sig_fcts += " " + sig_fct.m_fct_name + " |";
    }
    for (const auto &bkg_fct : pred_link.m_fcts_links_bkg) {
      bkg_fcts += " " + bkg_fct.m_fct_name + " |";
    }
    spdlog::debug("-> Sig fcts: {}", sig_fcts);
    spdlog::debug("-> Bkg fcts: {}", bkg_fcts);
  }
}

//------------------------------------------------------------------------------

} // namespace Setups
} // namespace PrEWUtils