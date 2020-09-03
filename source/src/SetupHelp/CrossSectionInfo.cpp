#include <DataHelp/PredDistrHelp.h>
#include <Names/CoefNaming.h>
#include <Names/FctNaming.h>
#include <Names/ParNaming.h>
#include <SetupHelp/CrossSectionInfo.h>

// includes from PrEW
#include <Data/DistrUtils.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

CrossSectionInfo::CrossSectionInfo(
    const std::string &distr_name,
    const std::vector<std::string> &chiral_configs)
    : m_distr_name(distr_name), m_chiral_configs(chiral_configs) {
  if (m_chiral_configs.size() == 0) {
    throw std::invalid_argument("ChiAsymmInfo:"
                                " Got empty vector for chiral configs!");
  }
}

//------------------------------------------------------------------------------

void CrossSectionInfo::use_chiral_asymmetries(
    const std::vector<std::string> &asymm_names) {
  /** Activate all allowed chiral asymmetries.
   **/
  m_using_asymms = true;
  m_custom_asymm_names = asymm_names;
  this->add_asymm_pars();
  this->add_asymm_pred_links();
}

void CrossSectionInfo::use_total_chiral_cross_section() {
  /** Activate the total (unpolarised) chiral cross section parameter.
   **/
  // Create the parameter
  m_pars.push_back(PrEW::Fit::FitPar(
      Names::ParNaming::total_chi_xs_par_name(m_distr_name), 1.0, 0.001));

  // Create the necessar function link
  PrEW::Data::FctLink fct_link{
      "Constant", {Names::ParNaming::total_chi_xs_par_name(m_distr_name)}, {}};

  // Create the prediction links (correct energy inserted later)
  for (const auto &chiral_config : m_chiral_configs) {
    PrEW::Data::DistrInfo info{m_distr_name, chiral_config, 0};
    PrEW::Data::PredLink pred_link{info, {fct_link}, {}};
    m_pred_links.push_back(pred_link);
  }
}

//------------------------------------------------------------------------------

const PrEW::Fit::ParVec &CrossSectionInfo::get_pars() const { return m_pars; }

PrEW::Data::PredLinkVec
CrossSectionInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
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
CrossSectionInfo::get_coefs(const PrEW::Data::PredDistrVec &preds) const {
  /** Get all the coefficients needed for the function related to the chiral
      cross sections.
   **/
  auto coefs = m_coefs;
  // Need to add chiral cross section coefs if asymmetries are activated
  if (m_using_asymms) {
    auto xs_coefs = this->get_xs_coefs(preds);
    coefs.insert(coefs.end(), xs_coefs.begin(), xs_coefs.end());
  }
  return coefs;
}

//------------------------------------------------------------------------------
// Internal funcitons
//------------------------------------------------------------------------------

void CrossSectionInfo::add_asymm_pars() {
  /** Add the parameters associated with the asymmetries between the given
      chiral configs.
   **/
  for (const auto & par_name : this->asymm_par_names()) {
    m_pars.push_back(PrEW::Fit::FitPar(par_name, 0, 0.0001));
  }
}

//------------------------------------------------------------------------------

void CrossSectionInfo::add_asymm_pred_links() {
  /** Create all the chiral asymmetry prediction links.
   **/
  for (const auto &chiral_config : m_chiral_configs) {
    // Get all the info needed to create the instructions
    int chiral_config_index = this->chiral_config_index(chiral_config);
    std::string fct_name = Names::FctNaming::chiral_asymm_name(
        chiral_config_index, m_chiral_configs.size());
    auto par_names = this->asymm_par_names();
    auto coef_names = this->xs_coef_names();

    // Function building instructions
    PrEW::Data::FctLink fct_link{fct_name, par_names, coef_names};

    // Energy specified later
    PrEW::Data::DistrInfo info{m_distr_name, chiral_config, 0};
    m_pred_links.push_back(PrEW::Data::PredLink{info, {fct_link}, {}});
  }
}

//------------------------------------------------------------------------------

PrEW::Data::CoefDistrVec
CrossSectionInfo::get_xs_coefs(const PrEW::Data::PredDistrVec &preds) const {
  /** Create the coefficients needed for the chiral asymmetries.
   **/
  int energy = preds.at(0).get_info().m_energy;

  PrEW::Data::CoefDistrVec xs_coefs{};
  for (const auto &chiral_config : m_chiral_configs) {
    // Find the distribution for this config and get its total cross section
    PrEW::Data::DistrInfo info_xs{m_distr_name, chiral_config, energy};
    auto pred = PrEW::Data::DistrUtils::subvec_info(preds, info_xs).at(0);
    auto coef_name = Names::CoefNaming::chi_xs_coef_name(info_xs);
    auto xs_val = DataHelp::PredDistrHelp::get_pred_sum(pred, "signal");

    // Add cross section as coef to all distributions that need it
    for (const auto &other_config : m_chiral_configs) {
      PrEW::Data::DistrInfo info_affected{m_distr_name, other_config, energy};
      xs_coefs.push_back(
          PrEW::Data::CoefDistr(coef_name, info_affected, xs_val));
    }
  }

  return xs_coefs;
}

//------------------------------------------------------------------------------=

int CrossSectionInfo::chiral_config_index(
    const std::string &chiral_config) const {
  /** Find the index of the given chiral configuration wrt. this asymmetry.
   **/
  auto chiral_config_it = std::find(m_chiral_configs.begin(),
                                    m_chiral_configs.end(), chiral_config);
  int chiral_config_index =
      std::distance(m_chiral_configs.begin(), chiral_config_it);
  return chiral_config_index;
}

//------------------------------------------------------------------------------

std::vector<std::string> CrossSectionInfo::xs_coef_names() const {
  /** Determine the names of the chiral cross section coefficients for this
      asymmetry using the naming conventions.
  **/
  std::vector<std::string> coef_names{};
  for (const auto &chiral_config : m_chiral_configs) {
    coef_names.push_back(
        Names::CoefNaming::chi_xs_coef_name({m_distr_name, chiral_config, 0}));
  }
  return coef_names;
}

//------------------------------------------------------------------------------

std::vector<std::string> CrossSectionInfo::asymm_par_names() const {
  /** Get the names of the parameters associated with the chiral asymmetries.
   **/
  std::vector<std::string> par_names{};

  int n_chiral_configs = m_chiral_configs.size();
  if (m_custom_asymm_names.size() != 0) {
    par_names = m_custom_asymm_names;
  } else if (n_chiral_configs == 2) {
    par_names.push_back(Names::ParNaming::asymm_par_name(m_distr_name));
  } else {
    // Multiple asymmetries
    for (int a = 1; a < n_chiral_configs; a++) {
      par_names.push_back(Names::ParNaming::asymm_par_name(m_distr_name, a));
    }
  }

  return par_names;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils