#include <DataHelp/FitParHelp.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------

PrEW::Fit::FitPar &FitParHelp::find_par_in_vec(const std::string &par_name,
                                               PrEW::Fit::ParVec &vec) {
  /** Find the fit parameter of given name in the given vector and return a
      (non-const) reference to it.
  **/
  auto name_cond = [par_name](const PrEW::Fit::FitPar &par) {
    return par.get_name() == par_name;
  };
  auto par_it = std::find_if(vec.begin(), vec.end(), name_cond);
  if (par_it == vec.end()) {
    throw std::invalid_argument("Couldn't find parameter " + par_name +
                                " in vector!");
  }
  return *par_it;
}

//------------------------------------------------------------------------------

void FitParHelp::add_par_to_vec(const PrEW::Fit::FitPar &par,
                                PrEW::Fit::ParVec &vec) {
  /** Add a given FitPar to the given vector if it is not already contained in
      the vector.
   **/
  auto cond = [par](const PrEW::Fit::FitPar &_par) { return _par == par; };
  auto par_it = std::find_if(vec.begin(), vec.end(), cond);
  if (par_it == vec.end()) {
    // Parameter not yet in vector -> Add it
    vec.push_back(par);
  } else {
    spdlog::debug("Parameter {} exists already, skipping it.", par.get_name());
  }
}

void FitParHelp::add_pars_to_vec(const PrEW::Fit::ParVec &pars,
                                 PrEW::Fit::ParVec &vec) {
  /** Add the given FitPar's to the given vector if they are not already
      contained in the vector.
   **/
  for (const auto &par : pars) {
    add_par_to_vec(par, vec);
  }
}

//------------------------------------------------------------------------------

} // namespace DataHelp
} // namespace PrEWUtils