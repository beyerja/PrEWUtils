#include <DataHelp/DistrHelp.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------

std::vector<std::string>
DistrHelp::find_distr_names(const PrEW::Data::InfoVec &infos) {
  /** Find the list of all the distribution names contained in the informations.
   **/
  std::set<std::string> names{}; // Use set to avoid duplicates
  for (const auto &info : infos) {
    names.insert(info.m_distr_name);
  }
  // Convert set to vector and return
  return std::vector<std::string>(names.begin(), names.end());
}


//------------------------------------------------------------------------------

} // namespace DataHelp
} // namespace PrEWUtils