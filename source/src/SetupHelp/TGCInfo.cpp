#include <SetupHelp/TGCInfo.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

TGCInfo::TGCInfo(const std::string &mode) : m_mode(mode) {}

//------------------------------------------------------------------------------
// Access functions

const std::string &TGCInfo::get_mode() const { return m_mode; }

//------------------------------------------------------------------------------
// Functions to use asymmetry in PrEW

PrEW::Fit::ParVec TGCInfo::get_pars(double initial_val,
                                    double initial_unc) const {
  /** Get the FitPar's for the TGCs.
   **/
  PrEW::Fit::ParVec pars{{"Delta-g1Z", initial_val, initial_unc},
                         {"Delta-kappa_gamma", initial_val, initial_unc},
                         {"Delta-lambda_gamma", initial_val, initial_unc}};
  return pars;
}

// TODO TODO TODO TODO
PrEW::Data::FctLink TGCInfo::get_fct_link() const {
  /** Get PrEW function link for the TGC parametrisation.
      These function links are used by PrEW as instructions for how to set up
      the bin predictions.
      The TGC coefficient names are fixed by Roberts naming choices in the ROOT
      files.
   **/

  // Name of the TGC parameters is independent of mode
  std::vector<std::string> par_names{"Delta-g1Z", "Delta-kappa_gamma",
                                     "Delta-lambda_gamma"};

  // Funciton name and coefficients are dependent on mode
  std::string fct_name{};
  std::vector<std::string> coef_names{};

  if (m_mode == "linear") {
    fct_name = "Linear3DPolynomial_Coeff";
    coef_names = {"One", "TGCA", "TGCB", "TGCC"};
  } else if (m_mode == "quadratic") {
    fct_name = "Quadratic3DPolynomial_Coeff";
    coef_names = {
        "One",  "TGCA", "TGCB", "TGCC", // linear
        "TGCD", "TGCE", "TGCF",         // quadratic
        "TGCG", "TGCH", "TGCI"          // mixed
    };
  } else {
    throw std::invalid_argument("TGCInfo: Unknown mode " + m_mode);
  }

  // Return instructions on how to build funciton for this chiral cross section
  return {fct_name, par_names, coef_names};
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils