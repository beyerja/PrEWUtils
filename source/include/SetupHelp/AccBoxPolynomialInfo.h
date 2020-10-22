#ifndef LIB_ACCBOXPOLYNOMIALINFO_H
#define LIB_ACCBOXPOLYNOMIALINFO_H 1

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

class AccBoxPolynomialInfo {
  /** Helper class that desribes the information connected to the
      parametrisation of an acceptance range that uses a 2nd order polynomial to
      describe the behaviour at the bin edges.
  **/

  std::string m_box_name{};
  std::string m_coord_name{};

  PrEW::Fit::ParVec m_pars{};
  PrEW::Data::CoefDistrVec m_coefs{};
  PrEW::Data::PredLinkVec m_pred_links{};

  std::vector<std::string> m_affected_distrs{};

public:
  // Constructors
  AccBoxPolynomialInfo(const std::string &box_name);

  // Adding information
  void fix_center();
  void fix_width();
  void add_distr(const std::string &distr_name);

  // Access functions
  const PrEW::Fit::ParVec &get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;

  PrEW::Data::FctLink get_fct_link() const;

  // Operators
  bool operator==(const AccBoxPolynomialInfo &other) const;

protected:
  // Internal functions
  std::string get_center_name() const;
  std::string get_width_name() const;

  PrEW::Fit::FitPar &find_par(const std::string &name);

  bool affects_distr(const PrEW::Data::DistrInfo &info) const;
};

using AccBoxPolynomialInfoVec = std::vector<AccBoxPolynomialInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif