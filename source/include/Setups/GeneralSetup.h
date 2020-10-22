#ifndef LIB_GENERALSETUP_H
#define LIB_GENERALSETUP_H 1

#include <SetupHelp/AccBoxInfo.h>
#include <SetupHelp/AccBoxPolynomialInfo.h>
#include <SetupHelp/ConstEffInfo.h>
#include <SetupHelp/CrossSectionInfo.h>
#include <SetupHelp/ParOrder.h>
#include <SetupHelp/RunInfo.h>
#include <SetupHelp/TGCInfo.h>

// Includes from PrEW
#include "Connect/DataConnector.h"
#include "Data/CoefDistr.h"
#include "Data/PolLink.h"
#include "Data/PredDistr.h"
#include "Data/PredLink.h"
#include "Fit/FitPar.h"

#include <map>
#include <string>

namespace PrEWUtils {
namespace Setups {

class GeneralSetup {
  /** Class to read the files provided by Robert Karl and use the output in
      PrEW.
      Supposed to work as usable interface for understandle execution of fits.
  **/
  int m_energy{};
  SetupHelp::RunInfo m_run{};

  // All data collected from input files
  PrEW::Data::PredDistrVec m_input_distrs{};
  PrEW::Data::CoefDistrVec m_input_coefs{};

  // Data & instructions to be used in fit
  PrEW::Data::PredDistrVec m_used_distrs{};
  PrEW::Data::CoefDistrVec m_used_coefs{};
  PrEW::Data::PredLinkVec m_pred_links{};
  PrEW::Fit::ParVec m_pars{};

  // Optional setup specifiers
  SetupHelp::AccBoxInfoVec m_acc_box_infos{};
  SetupHelp::AccBoxPolynomialInfoVec m_acc_box_polyn_infos{};
  SetupHelp::ConstEffInfoVec m_const_eff_infos{};
  SetupHelp::CrossSectionInfoVec m_xsection_infos{};
  SetupHelp::TGCInfoVec m_TGC_infos{};

  // Output specifiers
  SetupHelp::ParOrder::Ordering m_par_ordering{
      SetupHelp::ParOrder::default_ordering};
  SetupHelp::ParOrder::IDMap m_par_id_map{SetupHelp::ParOrder::default_par_map};

public:
  // Constructor
  GeneralSetup(int energy);

  // Add input
  void add_input_file(const std::string &file_path,
                      const std::string &file_type);
  void add_input_files(const std::string &dir, const std::string &file_name,
                       const std::string &file_type);

  // Functions determining how setup looks
  void use_distr(const std::string &distr_name,
                 const std::string &mode = "differential");

  void set_run(SetupHelp::RunInfo run_info);

  void add(SetupHelp::AccBoxInfo info);
  void add(SetupHelp::AccBoxPolynomialInfo info);
  void add(SetupHelp::ConstEffInfo info);
  void add(SetupHelp::CrossSectionInfo info);
  void add(SetupHelp::TGCInfo info);
  // void add(SetupHelp::ConstEffInfo info);

  void set_par_ordering(const SetupHelp::ParOrder::Ordering &ordering,
                        const SetupHelp::ParOrder::IDMap &id_map =
                            SetupHelp::ParOrder::default_par_map);

  // Finishing the setup
  void complete_setup();

  // Get result
  int get_energy() const;
  std::vector<int> get_energies() const;
  PrEW::Connect::DataConnector get_data_connector() const;

  const PrEW::Fit::ParVec &get_pars() const;
  const PrEW::Fit::ParVec &get_pars(int energy) const;

protected:
  PrEW::Data::PredDistrVec find_input_distrs(const std::string &distr_name);
  PrEW::Data::CoefDistrVec find_input_coefs(const std::string &distr_name);

  void add_coefs(const PrEW::Data::CoefDistrVec &coefs);
  void add_pars(const PrEW::Fit::ParVec &pars);
  void add_pred_links(const PrEW::Data::PredLinkVec &pred_links);

  void print_result() const;

  void complete_run_setup(const PrEW::Data::InfoVec &infos);
  void complete_acc_box_setup(const PrEW::Data::InfoVec &infos);
  void complete_acc_box_polyn_setup(const PrEW::Data::InfoVec &infos);
  void complete_const_eff_setup(const PrEW::Data::InfoVec &infos);
  void complete_TGC_setup(const PrEW::Data::InfoVec &infos);
  void complete_xsection_setup(const PrEW::Data::InfoVec &infos);

  void order_pars();
};

} // Namespace Setups
} // Namespace PrEWUtils

#endif