#ifndef LIB_ACCBOXINFO_H
#define LIB_ACCBOXINFO_H 1

// Includes from PrEW
#include <Data/CoefDistr.h>
#include <Data/FctLink.h>
#include <Data/PolLink.h>
#include <Data/PredLink.h>
#include <Fit/FitPar.h>

// Standard library
#include <map>
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

class AccBoxInfo {
  /** Helper class that desribes the information connected to the
      parametrisation of an acceptance range.
  **/

  std::string m_name_base{};
  std::string m_coord_name{};
  
  PrEW::Fit::ParVec m_pars{};
  PrEW::Data::CoefDistrVec m_coefs{};
  PrEW::Data::PredLinkVec m_pred_links{};

  std::vector<std::string> m_affected_distrs {};
  
public:
  // Constructors
  AccBoxInfo(const std::string &name_base, const std::string &coord_name, double center, double width);
  
  // Adding information
  void fix_center();
  void fix_width();
  void add_distr(const std::string &distr_name, int coord_index = 0,
                 double bin_width = 0.1);
  
  // Access functions
  const PrEW::Fit::ParVec &get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec
  get_coefs(const PrEW::Data::InfoVec &infos) const;
  
  PrEW::Data::FctLink get_fct_link() const;
  
  // Operators
  bool operator==(const AccBoxInfo &other) const;

protected:
  // Internal functions
  std::string get_center_name() const;
  std::string get_width_name() const;
  
  PrEW::Fit::FitPar &find_par(const std::string &name);
  
  bool affects_distr(const PrEW::Data::DistrInfo &info) const;
};

using AccBoxInfoVec = std::vector<AccBoxInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif