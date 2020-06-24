#ifndef LIB_AFINFO_H
#define LIB_AFINFO_H 1

// Includes from PrEW
#include <Data/FctLink.h>
#include <Fit/FitPar.h>
#include <GlobalVar/Chiral.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

class AfInfo {
  /** Helper class that desribes the information connected to an Af parameter
      for a 2-fermion distribution.
      Af = [ (c_L^f)^2 - (c_R^f)^2 ] / [ (c_L^f)^2 + (c_R^f)^2 ]
  **/

  std::string m_distr_name{};
  std::string m_Af_name{};
  std::string m_LR_config{};
  std::string m_RL_config{};

public:
  // Constructors
  AfInfo(const std::string &distr_name,
         const std::string &Af_name = "default",
         const std::string &LR_config = PrEW::GlobalVar::Chiral::eLpR,
         const std::string &RL_config = PrEW::GlobalVar::Chiral::eRpL);

  // Access functions
  const std::string &get_distr_name() const;
  const std::string &get_Af_name() const;
  const std::string &get_LR_config() const;
  const std::string &get_RL_config() const;

  // Functions needed for setting up PrEW fit
  PrEW::Fit::FitPar get_par(double initial_val = 0,
                            double initial_unc = 0.0001) const;
  PrEW::Data::FctLink get_fct_link(int energy,
                                   const std::string &chiral_config) const;

protected:
  // Internal functions
  // int chiral_config_index(const std::string &chiral_config) const;
  // std::vector<std::string> xs_coef_names(int energy) const;
};

} // namespace SetupHelp
} // namespace PrEWUtils

#endif