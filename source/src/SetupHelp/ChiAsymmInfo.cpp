#include <Names/CoefNaming.h>
#include <Names/FctNaming.h>
#include <Names/ParNaming.h>
#include <SetupHelp/ChiAsymmInfo.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

ChiAsymmInfo::ChiAsymmInfo(const std::string &distr_name,
                           const std::vector<std::string> &chiral_configs,
                           const std::vector<std::string> &par_names)
    : m_distr_name(distr_name), m_chiral_configs(chiral_configs),
      m_par_names(par_names), m_n_asymms(m_chiral_configs.size() - 1) {
  /** Constructor copying the given variables.
      If no asymmetry parameter names are provided then the default asymmetry
      parameter names are used.
  **/

  // Check that chiral configs were given
  if (m_chiral_configs.size() == 0) {
    throw std::invalid_argument("ChiAsymmInfo:"
                                " Got empty vector for chiral configs!");
  }

  // Check if par_names are given and if their length is as expected.
  // It either isn't true use the default naming.
  if ((m_par_names == std::vector<std::string>()) ||
      (m_par_names.size() != m_chiral_configs.size() - 1)) {
    if (m_n_asymms == 1) {
      // Only one asymmetry
      m_par_names = {Names::ParNaming::asymm_par_name(m_distr_name)};
    } else {
      // Multiple asymmetries
      for (size_t a = 1; a < m_chiral_configs.size(); a++) {
        m_par_names.push_back(
            Names::ParNaming::asymm_par_name(m_distr_name, a));
      }
    }
  }
}

//------------------------------------------------------------------------------
// Access functions

const std::string &ChiAsymmInfo::get_distr_name() const { return m_distr_name; }

const std::vector<std::string> &ChiAsymmInfo::get_chiral_configs() const {
  return m_chiral_configs;
}

const std::vector<std::string> &ChiAsymmInfo::get_par_names() const {
  return m_par_names;
}

int ChiAsymmInfo::get_n_asymms() const { return m_n_asymms; }

//------------------------------------------------------------------------------
// Functions to use asymmetry in PrEW

PrEW::Fit::ParVec ChiAsymmInfo::get_pars(double initial_val,
                                         double initial_unc) const {
  /** Get the FitPar's for this asymmetry.
   **/
  PrEW::Fit::ParVec pars{};
  for (const auto &par_name : m_par_names) {
    pars.push_back(PrEW::Fit::FitPar(par_name, initial_val, initial_unc));
  }
  return pars;
}

PrEW::Data::FctLink
ChiAsymmInfo::get_fct_link(int energy, const std::string &chiral_config) const {
  /** Get PrEW function link for a single chiral configuration.
      These function links are used by PrEW as instructions for how to set up
      the bin predictions.
   **/

  // Determine which function to use
  // -> Depends on place of chiral cross section in asymmetry
  int chiral_config_index = this->chiral_config_index(chiral_config);
  std::string fct_name =
      Names::FctNaming::chiral_asymm_name(chiral_config_index, m_n_asymms+1);

  // Get the coefficients needed for the function (chiral cross sections)
  std::vector<std::string> coef_names = this->xs_coef_names(energy);

  // Return instructions on how to build funciton for this chiral cross section
  return {fct_name, m_par_names, coef_names};
}

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

int ChiAsymmInfo::chiral_config_index(const std::string &chiral_config) const {
  /** Find the index of the given chiral configuration wrt. this asymmetry.
   **/
  auto chiral_config_it = std::find(m_chiral_configs.begin(),
                                    m_chiral_configs.end(), chiral_config);
  int chiral_config_index =
      std::distance(m_chiral_configs.begin(), chiral_config_it);
  return chiral_config_index;
}

std::vector<std::string> ChiAsymmInfo::xs_coef_names(int energy) const {
  /** Determine the names of the chiral cross section coefficients for this
      asymmetry using the naming conventions.
  **/
  std::vector<std::string> coef_names{};
  for (const auto &chiral_config : m_chiral_configs) {
    coef_names.push_back(Names::CoefNaming::chi_xs_coef_name(
        {m_distr_name, chiral_config, energy}));
  }
  return coef_names;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils