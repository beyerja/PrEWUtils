#include <DataHelp/CoefDistrHelp.h>
#include <DataHelp/DistrHelp.h>
#include <DataHelp/FitParHelp.h>
#include <DataHelp/PredLinkHelp.h>
#include <SetupHelp/InputHelp.h>
#include <Setups/GeneralSetup.h>

// Includes from PrEW
#include <Data/DistrUtils.h>
#include <Input/DataReader.h>
#include <Input/InfoRKFile.h>
#include <Input/InputInfo.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace Setups {

//------------------------------------------------------------------------------

GeneralSetup::GeneralSetup(int energy) : m_energy(energy) {}

//------------------------------------------------------------------------------

void GeneralSetup::add_input_file(const std::string &file_path,
                                  const std::string &file_type) {
  /** Read the distributions and coefficients from the file which is of the
      given input type.
   **/

  // Read the file
  PrEW::Input::InputInfo *info{};
  if (file_type == "RK") {
    info = new PrEW::Input::InfoRKFile{{file_path, "RK"}, m_energy};
  } else if (file_type == "CSV") {
    info = new PrEW::Input::InputInfo{file_path, "CSV"};
  } else {
    throw std::invalid_argument("Unknown input file type" + file_type);
  }
  PrEW::Input::DataReader reader(info);
  reader.read_file();

  // Use the new info from the file
  auto new_pred = reader.get_pred_distrs();
  auto new_coef = reader.get_coef_distrs();
  // Adding results to input vectors for later use
  m_input_distrs.insert(m_input_distrs.end(), new_pred.begin(), new_pred.end());
  m_input_coefs.insert(m_input_coefs.end(), new_coef.begin(), new_coef.end());

  delete info;
}

//------------------------------------------------------------------------------

void GeneralSetup::add_input_files(const std::string &dir,
                                   const std::string &file_name,
                                   const std::string &file_type) {
  /** Read distributions from the files in the directory that fit the given file
      name.
      The file name can contain regular expressions.
   **/

  auto file_paths = SetupHelp::InputHelp::regex_search(dir, file_name);

  if (file_paths.size() == 0) {
    spdlog::warn("No files found in {} that fit {}", dir, file_name);
  }

  for (const auto &file_path : file_paths) {
    this->add_input_file(file_path, file_type);
  }
}

//------------------------------------------------------------------------------

void GeneralSetup::use_distr(const std::string &distr_name,
                             const std::string &mode) {
  /** Select a distribution to be used in the fit and the mode in which it is
      to be used.
      Examples:
        "singleWplussemileptonic"
        "singleWminussemileptonic"
        "WW_semilep_MuAntiNu"
        "WW_semilep_AntiMuNu"
        ... (Depending on file contents.)
      Possible modes:
        "differential"
        "summed"
  **/
  auto distrs = this->find_input_distrs(distr_name);
  auto coefs = this->find_input_coefs(distr_name);

  if (distrs.size() == 0) {
    spdlog::warn("Didn't find any predictions for distribution: {}",
                 distr_name);
  } else {
    spdlog::debug("Found {} chiral distrs for {}", distrs.size(), distr_name);
  }

  if (mode == "differential") {
    // Nothing to do, already differential
  } else if (mode == "summed") {
    // Request use of summed up / combined bins (e.g. total xs)
    // -> Transform each distr. into it's total xs only
    distrs = PrEW::Data::DistrUtils::combine_bins(distrs);
    if (coefs.size() > 0) {
      spdlog::warn("Coefs for summed distr {} removed!", distr_name);
      coefs = {};
    }
  } else {
    spdlog::warn("Unknown distribution mode {} for distribution {}.\n"
                 "-> Will use default differential distribution.",
                 distr_name, mode);
  }

  m_used_distrs.insert(m_used_distrs.end(), distrs.begin(), distrs.end());
  m_used_coefs.insert(m_used_coefs.end(), coefs.begin(), coefs.end());
}

//------------------------------------------------------------------------------

void GeneralSetup::set_run(SetupHelp::RunInfo run_info) {
  /** Set the run that is to be tested in this setup.
   **/
  m_run = run_info;
}

//------------------------------------------------------------------------------

void GeneralSetup::add(SetupHelp::AccBoxInfo info) {
  /** Add an acceptance box.
   **/
  m_acc_box_infos.push_back(info);
}

void GeneralSetup::add(SetupHelp::ConstEffInfo info) {
  /** Add an chiral cross section instruction.
   **/
  m_const_eff_infos.push_back(info);
}

void GeneralSetup::add(SetupHelp::TGCInfo info) {
  /** Add an chiral cross section instruction.
   **/
  m_TGC_infos.push_back(info);
}

void GeneralSetup::add(SetupHelp::CrossSectionInfo info) {
  /** Add an chiral cross section instruction.
   **/
  m_xsection_infos.push_back(info);
}

//------------------------------------------------------------------------------

void GeneralSetup::set_par_ordering(
    const SetupHelp::ParOrder::Ordering &ordering,
    const SetupHelp::ParOrder::IDMap &id_map) {
  /** Specify an ordering for the output parameters
   **/
  m_par_ordering = ordering;
  m_par_id_map = id_map;
}

//------------------------------------------------------------------------------

void GeneralSetup::complete_setup() {
  /** Set up the function links and coefficients according to the
      configurations provided previously. Creates all the function linking
      instructions that are needed to set up a proper running fit.
   **/
  auto infos = DataHelp::DistrHelp::find_infos(m_used_distrs);

  spdlog::debug("Completing individual setups.");

  this->complete_run_setup(infos);
  this->complete_acc_box_setup(infos);
  this->complete_const_eff_setup(infos);
  this->complete_TGC_setup(infos);
  this->complete_xsection_setup(infos);

  spdlog::debug("Reordering parameters.");
  this->order_pars();

  this->print_result(); // Result printing in debug mode
}

//------------------------------------------------------------------------------

int GeneralSetup::get_energy() const { return m_energy; }
std::vector<int> GeneralSetup::get_energies() const { return {m_energy}; }
const PrEW::Fit::ParVec &GeneralSetup::get_pars() const { return m_pars; }

const PrEW::Fit::ParVec &GeneralSetup::get_pars(int energy) const {
  /** Parameters are not energy-dependent here since setup is energy-specific.
   **/
  if (energy != m_energy) {
    throw std::invalid_argument("Requesting wrong energy" +
                                std::to_string(energy));
  }
  return m_pars;
}

//------------------------------------------------------------------------------

PrEW::Connect::DataConnector GeneralSetup::get_data_connector() const {
  /** Get the connector that contains all the information that it needs to
      properly link the predicition functions.
      Must be called _after_ calling final combination.
  **/
  return PrEW::Connect::DataConnector(m_used_distrs, m_used_coefs, m_pred_links,
                                      m_run.get_pol_links());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

PrEW::Data::PredDistrVec
GeneralSetup::find_input_distrs(const std::string &distr_name) {
  /** Find the input prediction distribtuions to the given distribution name.
   **/
  return PrEW::Data::DistrUtils::subvec_energy_and_name(m_input_distrs,
                                                        m_energy, distr_name);
}

PrEW::Data::CoefDistrVec
GeneralSetup::find_input_coefs(const std::string &distr_name) {
  /** Find the input coefficients to the given distribution name.
   **/
  return PrEW::Data::DistrUtils::subvec_energy_and_name(m_input_coefs, m_energy,
                                                        distr_name);
}

//------------------------------------------------------------------------------

void GeneralSetup::add_coefs(const PrEW::Data::CoefDistrVec &coefs) {
  /** Add a coefficient to this setup
   **/
  DataHelp::CoefDistrHelp::add_coefs_to_vec(coefs, m_used_coefs);
}

//------------------------------------------------------------------------------

void GeneralSetup::add_pars(const PrEW::Fit::ParVec &pars) {
  /** Add the given parameters to the setup parameters.
   **/
  DataHelp::FitParHelp::add_pars_to_vec(pars, m_pars);
}

//------------------------------------------------------------------------------

void GeneralSetup::add_pred_links(const PrEW::Data::PredLinkVec &pred_links) {
  /** Add the prediction links to the links of this setup.
   **/
  spdlog::debug("Trying to add {} prediction links.", pred_links.size());
  DataHelp::PredLinkHelp::add_links_to_vec(pred_links, m_pred_links);
}

//------------------------------------------------------------------------------

void GeneralSetup::print_result() const {
  /** Print the result of the complete setup for debugging.
   **/
  spdlog::debug("GeneralSetup: Printing results of completed setup");
  spdlog::debug("Using Parameters:");
  std::string pars{" "};
  for (const auto &par : m_pars) {
    pars += " " + par.get_name() + " |";
  }
  spdlog::debug(pars);

  spdlog::debug("Using distributions:");
  for (const auto &used_distr : m_used_distrs) {
    const auto &info = used_distr.m_info;
    spdlog::debug("{} @ {} & {}", info.m_distr_name, info.m_energy,
                  info.m_pol_config);
    spdlog::debug(" -> First signal value: {}", used_distr.m_sig_distr.at(0));
  }
  spdlog::debug("Using coefficients:");
  for (const auto &used_coef : m_used_coefs) {
    const auto &info = used_coef.get_info();
    spdlog::debug("{} for {} @ {} & {}", used_coef.get_coef_name(),
                  info.m_distr_name, info.m_energy, info.m_pol_config);
    spdlog::debug(" -> First coef value: {}", used_coef.get_coef(0));
  }
  spdlog::debug("Using prediction links:");
  for (const auto &pred_link : m_pred_links) {
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

void GeneralSetup::complete_run_setup(const PrEW::Data::InfoVec &infos) {
  /** Complete the part of the setup related to the collider run parameters.
   **/
  this->add_pars(m_run.get_pars());
  this->add_pred_links(m_run.get_pred_links(infos));
  this->add_coefs(m_run.get_coefs(infos));
}

void GeneralSetup::complete_acc_box_setup(const PrEW::Data::InfoVec &infos) {
  /** Complete the part of the setup related to the acceptance boxes.
   **/
  for (const auto &acc_box : m_acc_box_infos) {
    this->add_pars(acc_box.get_pars());
    this->add_pred_links(acc_box.get_pred_links(infos));
    this->add_coefs(acc_box.get_coefs(infos));
  }
}

void GeneralSetup::complete_const_eff_setup(const PrEW::Data::InfoVec &infos) {
  /** Complete the part of the setup related to constant efficiencies.
   **/
  for (const auto &const_eff : m_const_eff_infos) {
    this->add_pars(const_eff.get_pars());
    this->add_pred_links(const_eff.get_pred_links(infos));
  }
}

void GeneralSetup::complete_TGC_setup(const PrEW::Data::InfoVec &infos) {
  /** Complete the part of the setup related to the acceptance boxes.
   **/
  for (const auto &TGC_info : m_TGC_infos) {
    this->add_pars(TGC_info.get_pars());
    this->add_pred_links(TGC_info.get_pred_links(infos));
    this->add_coefs(TGC_info.get_coefs(infos));
  }
}

void GeneralSetup::complete_xsection_setup(const PrEW::Data::InfoVec &infos) {
  /** Complete the part of the setup related to chiral cross sections.
   **/
  for (const auto &xsection_info : m_xsection_infos) {
    this->add_pars(xsection_info.get_pars());
    this->add_pred_links(xsection_info.get_pred_links(infos));
    this->add_coefs(xsection_info.get_coefs(m_used_distrs));
  }
}

//------------------------------------------------------------------------------

void GeneralSetup::order_pars() {
  /** Order the parameters according to the specification.
   **/
  m_pars =
      SetupHelp::ParOrder::reorder_pars(m_pars, m_par_ordering, m_par_id_map);
}

//------------------------------------------------------------------------------

} // namespace Setups
} // Namespace PrEWUtils