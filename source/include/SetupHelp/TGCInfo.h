#ifndef LIB_TGCINFO_H
#define LIB_TGCINFO_H 1

// Includes from PrEW
#include <Data/FctLink.h>
#include <Fit/FitPar.h>

// Standard library
#include <string>

namespace PrEWUtils {
namespace SetupHelp {

class TGCInfo {
  /** Helper class that desribes the information connected to the TGC
      parametrisation.
  **/

  std::string m_mode{};

public:
  // Constructors
  TGCInfo(const std::string &mode = "linear");

  // Access functions
  const std::string &get_mode() const;

  // Functions needed for setting up PrEW fit
  PrEW::Fit::ParVec get_pars(double initial_val = 0,
                             double initial_unc = 0.0001) const;
  PrEW::Data::FctLink get_fct_link() const;
};

} // namespace SetupHelp
} // namespace PrEWUtils

#endif