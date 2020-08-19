#include <Names/ParNaming.h>
#include <SetupHelp/ConstEffInfo.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

ConstEffInfo::ConstEffInfo(const std::string &distr_name, double eff)
    : m_distr_name(distr_name), m_eff(eff) {}

//------------------------------------------------------------------------------
// Modification funcitons

void ConstEffInfo::fix() { m_is_fixed = true; }

//------------------------------------------------------------------------------
// Access functions

// Access functions
const std::string &ConstEffInfo::get_distr_name() const { return m_distr_name; }
double ConstEffInfo::get_eff() const { return m_eff; }

//------------------------------------------------------------------------------
// Functions to use asymmetry in PrEW

PrEW::Fit::FitPar ConstEffInfo::get_par() const {
  /** Get the PrEW FitPar for this Af parameter.
   **/
  PrEW::Fit::FitPar eff_par(Names::ParNaming::const_eff_name(m_distr_name),
                            m_eff, 0.0001 * m_eff);
  if (m_is_fixed) {
    eff_par.fix();
  }
  return eff_par;
}

PrEW::Data::FctLink ConstEffInfo::get_fct_link() const {
  /** Get PrEW function link for a single chiral configuration.
      These function links are used by PrEW as instructions for how to set up
      the bin predictions.
   **/
  PrEW::Data::FctLink fct_link{
      "Constant",                                       // PrEW function name
      {Names::ParNaming::const_eff_name(m_distr_name)}, // Parameter names
      {}                                                // no coefficients
  };
  return fct_link;
}

//------------------------------------------------------------------------------
// Operators

bool ConstEffInfo::operator==(const ConstEffInfo &other) const {
  return m_distr_name == other.get_distr_name();
}

bool ConstEffInfo::operator==(const std::string &distr_name) const {
  return m_distr_name == distr_name;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils