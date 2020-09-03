#include <Names/ParNaming.h>
#include <SetupHelp/ConstEffInfo.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

ConstEffInfo::ConstEffInfo(const std::string &distr_name, double eff)
    : m_distr_name(distr_name),
      m_eff_par(Names::ParNaming::const_eff_name(distr_name), eff,
                0.0001 * eff) {}

//------------------------------------------------------------------------------
// Modification funcitons

void ConstEffInfo::fix() { m_eff_par.fix(); }

void ConstEffInfo::constrain(double val, double unc) {
  /** Set a gaussian constraint on the efficiency parameter.
   **/
  m_eff_par.set_constrgauss(val, unc);
}

//------------------------------------------------------------------------------
// Access functions

PrEW::Fit::ParVec ConstEffInfo::get_pars() const {
  /** Get the PrEW FitPar for this efficiency parameter.
   **/
  return {m_eff_par};
}

PrEW::Data::PredLinkVec
ConstEffInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
  /** Get the linking instructions for PrEW.
   **/
  PrEW::Data::PredLinkVec pred_links{};

  // The relevant function instruction
  PrEW::Data::FctLink fct_link{
      "Constant",                                       // PrEW function name
      {Names::ParNaming::const_eff_name(m_distr_name)}, // Parameter names
      {}                                                // no coefficients
  };

  // Use the function instruction for the affected distribution
  for (const auto &info : infos) {
    if (info.m_distr_name == m_distr_name) {
      pred_links.push_back(PrEW::Data::PredLink{info, {fct_link}, {}});
    }
  }
  return pred_links;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils