#include <DataHelp/FitParHelp.h>
#include <Names/CoefNaming.h>
#include <SetupHelp/AccBoxInfo.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

AccBoxInfo::AccBoxInfo(const std::string &name_base,
                       const std::string &coord_name, double center,
                       double width)
    : m_name_base(name_base), m_coord_name(coord_name) {
  // Create the center and width parameters
  m_pars.push_back(PrEW::Fit::FitPar(this->get_center_name(), center, 0.0001));
  m_pars.push_back(PrEW::Fit::FitPar(this->get_width_name(), width, 0.0001));
}

//------------------------------------------------------------------------------
// Adding information

void AccBoxInfo::add_distr(const std::string &distr_name, int coord_index,
                           double bin_width) {
  /** Add an affected distribution, the index of the relevant coordinate and the
      distribution bin width.
   **/
  PrEW::Data::DistrInfo named_info{distr_name, "", 0};

  m_coefs.push_back(
      PrEW::Data::CoefDistr(m_coord_name, named_info, coord_index));
  m_coefs.push_back(PrEW::Data::CoefDistr(
      Names::CoefNaming::bin_width_coef_name(), named_info, bin_width));

  m_affected_distrs.push_back(distr_name);
}

void AccBoxInfo::fix_center() {
  /** Fix the center parameter.
   **/
  (this->find_par(this->get_center_name())).fix();
}

void AccBoxInfo::fix_width() {
  /** Fix the width parameter.
   **/
  (this->find_par(this->get_width_name())).fix();
}

//------------------------------------------------------------------------------
// Access functions

const PrEW::Fit::ParVec &AccBoxInfo::get_pars() const { return m_pars; }

PrEW::Data::PredLinkVec
AccBoxInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
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

PrEW::Data::CoefDistrVec
AccBoxInfo::get_coefs(const PrEW::Data::InfoVec &infos) const {
  /** Return coefficients for all given distributions.
   **/
  PrEW::Data::CoefDistrVec coefs{};
  for (const auto &info : infos) {
    for (const auto &coef : m_coefs) {
      // If coef relevant to info (same distr name), specify coef and add
      if (coef.get_info().m_distr_name == info.m_distr_name) {
        auto coef_copy = coef;
        coef_copy.set_info(info);
        coefs.push_back(coef_copy);
      }
    }
  }
  return coefs;
}

PrEW::Data::FctLink AccBoxInfo::get_fct_link() const {
  /** Get PrEW function link for the acceptance box parametrisation.
      These function links are used by PrEW as instructions for how to set up
      the bin predictions.
   **/

  std::string fct_name = "AcceptanceBox";
  std::vector<std::string> par_names{this->get_center_name(),
                                     this->get_width_name()};
  std::vector<std::string> coef_names{m_coord_name,
                                      Names::CoefNaming::bin_width_coef_name()};

  return {fct_name, par_names, coef_names};
}

//------------------------------------------------------------------------------
// Internal functions

std::string AccBoxInfo::get_center_name() const {
  return m_name_base + "_center";
}
std::string AccBoxInfo::get_width_name() const {
  return m_name_base + "_width";
}

PrEW::Fit::FitPar &AccBoxInfo::find_par(const std::string &name) {
  /** Find the parameter of given name in the parameter vector.
   **/
  return DataHelp::FitParHelp::find_par_in_vec(name, m_pars);
}

bool AccBoxInfo::affects_distr(const PrEW::Data::DistrInfo &info) const {
  /** Check if the given distribution is affected by this box.
   **/
  auto name_it = std::find(m_affected_distrs.begin(), m_affected_distrs.end(),
                           info.m_distr_name);
  return name_it != m_affected_distrs.end();
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils