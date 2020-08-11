#include <Names/CoefNaming.h>
#include <SetupHelp/AccBoxInfo.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

AccBoxInfo::AccBoxInfo(const std::string &name_base,
                       const std::string &coord_name)
    : m_name_base(name_base), m_coord_name(coord_name) {}

//------------------------------------------------------------------------------
// Adding information

void AccBoxInfo::add_distr(const std::string &distr_name, int coord_index,
                           double bin_width) {
  /** Add an affected distribution, the index of the relevant coordinate and the
      distribution bin width.
   **/
  m_distrs[distr_name] = {coord_index, bin_width};
}

//------------------------------------------------------------------------------
// Access functions

const std::string &AccBoxInfo::get_name_base() const { return m_name_base; }
const std::string &AccBoxInfo::get_coord_name() const { return m_coord_name; }

bool AccBoxInfo::affects_distr(const std::string &distr_name) const {
  /** Check if the given distribution was registered to be affected by this
      acceptance.
   **/
  return m_distrs.find(distr_name) != m_distrs.end();
}

int AccBoxInfo::coord_index(const std::string &distr_name) const {
  /** Return the index of the relevant coordinate for this distribution.
   **/
  return m_distrs.at(distr_name).m_coord_index;
}

double AccBoxInfo::bin_width(const std::string &distr_name) const {
  /** Return the bin width for this distribution.
   **/
  return m_distrs.at(distr_name).m_bin_width;
}

//------------------------------------------------------------------------------
// Functions to use asymmetry in PrEW

PrEW::Fit::ParVec AccBoxInfo::get_pars(double initial_center,
                                       double initial_width,
                                       double initial_unc) const {
  /** Get the FitPar's for the acceptance box center and width.
   **/
  PrEW::Fit::ParVec pars{{this->get_center_name(), initial_center, initial_unc},
                         {this->get_width_name(), initial_width, initial_unc}};
  return pars;
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
// Operators

bool AccBoxInfo::operator==(const AccBoxInfo &other) const {
  /** Box identified by its name and its coordinate.
   **/
  return (m_name_base == other.get_name_base()) &&
         (m_coord_name == other.get_coord_name());
}

//------------------------------------------------------------------------------
// Internal functions

std::string AccBoxInfo::get_center_name() const {
  return m_name_base + "_center";
}
std::string AccBoxInfo::get_width_name() const {
  return m_name_base + "_width";
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils