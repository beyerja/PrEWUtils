#include <DataHelp/FitParHelp.h>
#include <Names/CoefNaming.h>
#include <SetupHelp/AccBoxPolynomialInfo.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

AccBoxPolynomialInfo::AccBoxPolynomialInfo(const std::string &box_name)
    : m_box_name(box_name) {
  // Create the center and width deviation parameters (initialised as zero)
  m_pars.push_back(PrEW::Fit::FitPar(this->get_center_name(), 0.0, 0.0001));
  m_pars.push_back(PrEW::Fit::FitPar(this->get_width_name(), 0.0, 0.0001));
}

//------------------------------------------------------------------------------
// Adding information

void AccBoxPolynomialInfo::add_distr(const std::string &distr_name) {
  /** Add an affected distribution.
   **/
  m_affected_distrs.push_back(distr_name);
}

void AccBoxPolynomialInfo::fix_center() {
  /** Fix the center parameter.
   **/
  (this->find_par(this->get_center_name())).fix();
}

void AccBoxPolynomialInfo::fix_width() {
  /** Fix the width parameter.
   **/
  (this->find_par(this->get_width_name())).fix();
}

//------------------------------------------------------------------------------
// Access functions

const PrEW::Fit::ParVec &AccBoxPolynomialInfo::get_pars() const {
  return m_pars;
}

PrEW::Data::PredLinkVec
AccBoxPolynomialInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
  /** Return all predicition links relevant to the given distributions.
   **/
  PrEW::Data::PredLinkVec links{};
  auto fct_link = this->get_fct_link(); // General function link for this box
  for (const auto &info : infos) {
    if (this->affects_distr(info)) {
      // Distribution is affected, create the link for it
      links.push_back(PrEW::Data::PredLink{info, {fct_link}, {fct_link}});
    }
  }
  return links;
}

PrEW::Data::FctLink AccBoxPolynomialInfo::get_fct_link() const {
  /** Get PrEW function link for the acceptance box parametrisation.
      These function links are used by PrEW as instructions for how to set up
      the bin predictions.
   **/

  std::string fct_name = "AcceptanceBoxPolynomial";
  std::vector<std::string> par_names{this->get_center_name(),
                                     this->get_width_name()};
  std::vector<std::string> coef_names{m_box_name + "_k0",  m_box_name + "_kc",
                                      m_box_name + "_kw",  m_box_name + "_kc2",
                                      m_box_name + "_kw2", m_box_name + "_kcw"};

  return {fct_name, par_names, coef_names};
}

//------------------------------------------------------------------------------
// Internal functions

std::string AccBoxPolynomialInfo::get_center_name() const {
  return m_box_name + "_dCenter";
}
std::string AccBoxPolynomialInfo::get_width_name() const {
  return m_box_name + "_dWidth";
}

PrEW::Fit::FitPar &AccBoxPolynomialInfo::find_par(const std::string &name) {
  /** Find the parameter of given name in the parameter vector.
   **/
  return DataHelp::FitParHelp::find_par_in_vec(name, m_pars);
}

bool AccBoxPolynomialInfo::affects_distr(
    const PrEW::Data::DistrInfo &info) const {
  /** Check if the given distribution is affected by this box.
   **/
  auto name_it = std::find(m_affected_distrs.begin(), m_affected_distrs.end(),
                           info.m_distr_name);
  return name_it != m_affected_distrs.end();
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils