#ifndef LIB_ACCBOXINFO_H
#define LIB_ACCBOXINFO_H 1

// Includes from PrEW
#include <Data/FctLink.h>
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
  
  // Internal trackers if the parameters are supposed to be fixed
  bool m_center_fixed{false};
  bool m_width_fixed{false};

  // Map of affected distributions and the index of the relevant coordinate
  struct AccBoxDistrInfo {
    // Internal struct to keep track of relevant distr data
    int m_coord_index{};
    double m_bin_width{};
  };
  std::map<std::string, AccBoxDistrInfo> m_distrs{};

public:
  // Constructors
  AccBoxInfo(const std::string &name_base, const std::string &coord_name);

  // Adding information
  void fix_center();
  void fix_width();
  void add_distr(const std::string &distr_name, int coord_index,
                 double bin_width);

  // Access functions
  const std::string &get_name_base() const;
  const std::string &get_coord_name() const;

  bool affects_distr(const std::string &distr_name) const;
  int coord_index(const std::string &distr_name) const;
  double bin_width(const std::string &distr_name) const;

  // Functions needed for setting up PrEW fit
  PrEW::Fit::ParVec get_pars(double initial_center = 0,
                             double initial_width = 0.9,
                             double initial_unc = 0.0001) const;
  PrEW::Data::FctLink get_fct_link() const;

  // Operators
  bool operator==(const AccBoxInfo &other) const;

protected:
  // Internal functions
  std::string get_center_name() const;
  std::string get_width_name() const;
};

} // namespace SetupHelp
} // namespace PrEWUtils

#endif