#ifndef LIB_FCTNAMING_H
#define LIB_FCTNAMING_H 1

// Includes from PrEW
#include "Data/DistrInfo.h"

#include <string>

namespace PrEWUtils {
namespace Names {

namespace FctNaming {
/** Namespace for some conventions of how functions are named in PrEW.
 **/

std::string chiral_asymm_name(int config_index, int n_configs);

} // Namespace FctNaming

} // Namespace Names
} // Namespace PrEWUtils

#endif