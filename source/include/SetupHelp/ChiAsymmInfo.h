#ifndef LIB_CHIASYMMINFO_H
#define LIB_CHIASYMMINFO_H 1

// Includes from PrEW
#include <Data/FctLink.h>
#include <Fit/FitPar.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

class ChiAsymmInfo {
  /** Helper struct that desribes the information connected to a  asymmetry
      for a distribution.
  **/

  std::string m_distr_name{};
  std::vector<std::string> m_chiral_configs{};
  std::vector<std::string> m_par_names{};
  int m_n_asymms{};

public:
  // Constructors
  ChiAsymmInfo(const std::string &distr_name,
               const std::vector<std::string> &chiral_configs,
               const std::vector<std::string> &par_names = {});

  // Access functions
  const std::string &get_distr_name() const;
  const std::vector<std::string> &get_chiral_configs() const;
  const std::vector<std::string> &get_par_names() const;
  int get_n_asymms() const;

  // Functions needed for setting up PrEW fit
  PrEW::Fit::ParVec get_pars(double initial_val = 0,
                             double initial_unc = 0.0001) const;
  PrEW::Data::FctLink get_fct_link(int energy,
                                   const std::string &chiral_config) const;

protected:
  // Internal functions
  int chiral_config_index(const std::string &chiral_config) const;
  std::vector<std::string> xs_coef_names(int energy) const;
};

} // namespace SetupHelp
} // namespace PrEWUtils

#endif