#ifndef LIB_CROSSSECTIONINFO_H
#define LIB_CROSSSECTIONINFO_H 1

// Includes from PrEW
#include <Data/CoefDistr.h>
#include <Data/PredDistr.h>
#include <Data/PredLink.h>
#include <Fit/FitPar.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

class CrossSectionInfo {
  /** Helper struct that desribes the information connected to a chiral cross
      section parameters and asymmetries.
  **/

  std::string m_distr_name{};
  std::vector<std::string> m_chiral_configs{};

  PrEW::Fit::ParVec m_pars{};
  PrEW::Data::CoefDistrVec m_coefs{};
  PrEW::Data::PredLinkVec m_pred_links{};

  // Internal trackers
  bool m_using_asymms{false};
  std::vector<std::string> m_custom_asymm_names{};

public:
  // Constructors
  CrossSectionInfo(const std::string &distr_name,
                   const std::vector<std::string> &chiral_configs);

  // Add information
  void use_chiral_asymmetries(const std::vector<std::string> &asymm_names = {});
  void use_total_chiral_cross_section();

  // Access functions
  const PrEW::Fit::ParVec &get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec
  get_coefs(const PrEW::Data::PredDistrVec &preds) const;

protected:
  // Internal functions
  void activate_asymmetry_2xs();
  void activate_asymmetries_3xs();

  void add_asymm_pars();
  void add_asymm_pred_links();

  PrEW::Data::CoefDistrVec
  get_xs_coefs(const PrEW::Data::PredDistrVec &preds) const;

  std::vector<std::string> xs_coef_names() const;
  std::vector<std::string> asymm_par_names() const;

  int chiral_config_index(const std::string &chiral_config) const;
};

using CrossSectionInfoVec = std::vector<CrossSectionInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif