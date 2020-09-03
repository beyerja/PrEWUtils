#include <DataHelp/CoefDistrHelp.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------

void CoefDistrHelp::add_coef_to_vec(const PrEW::Data::CoefDistr &coef,
                                    PrEW::Data::CoefDistrVec &vec) {
  /** Add a given coefficient to the given vector if it is not already contained
      in the vector.
   **/

  auto coef_it = std::find(vec.begin(), vec.end(), coef);
  if (coef_it == vec.end()) {
    vec.push_back(coef);
  } else {
    auto info = coef.get_info();
    spdlog::debug("Parameter {} exists already for {} {} @ {}, skipping it.",
                  coef.get_coef_name(), info.m_distr_name, info.m_pol_config,
                  info.m_energy);
  }
}

void CoefDistrHelp::add_coefs_to_vec(const PrEW::Data::CoefDistrVec &coefs,
                                     PrEW::Data::CoefDistrVec &vec) {
  /** Add the given coefficients to the given vector if they are not already
      contained in the vector.
   **/
  for (const auto &coef : coefs) {
    add_coef_to_vec(coef, vec);
  }
}

//------------------------------------------------------------------------------

} // namespace DataHelp
} // namespace PrEWUtils