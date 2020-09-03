#ifndef LIB_FITPARHELP_H
#define LIB_FITPARHELP_H 1

// includes from PrEW
#include <Fit/FitPar.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace DataHelp {

namespace FitParHelp {
/** Helper functions for anything related to PrEW fit parameters.
 **/

PrEW::Fit::FitPar &find_par_in_vec(const std::string &par_name,
                                   PrEW::Fit::ParVec &vec);
                                   
void add_par_to_vec(const PrEW::Fit::FitPar &par, PrEW::Fit::ParVec &vec);
void add_pars_to_vec(const PrEW::Fit::ParVec &pars, PrEW::Fit::ParVec &vec);

} // namespace FitParHelp

} // namespace DataHelp
} // namespace PrEWUtils

#endif