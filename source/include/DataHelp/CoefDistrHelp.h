#ifndef LIB_COEFDISTRHELP_H
#define LIB_COEFDISTRHELP_H 1

// includes from PrEW
#include <Data/CoefDistr.h>

namespace PrEWUtils {
namespace DataHelp {

namespace CoefDistrHelp {
/** Helper functions for anything related to PrEW coefficients.
 **/

void add_coef_to_vec(const PrEW::Data::CoefDistr &coef,
                     PrEW::Data::CoefDistrVec &vec);
void add_coefs_to_vec(const PrEW::Data::CoefDistrVec &coefs,
                      PrEW::Data::CoefDistrVec &vec);

} // namespace CoefDistrHelp

} // namespace DataHelp
} // namespace PrEWUtils

#endif