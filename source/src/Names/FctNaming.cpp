#include <Names/FctNaming.h>

namespace PrEWUtils {
namespace Names {

//------------------------------------------------------------------------------

std::string FctNaming::chiral_asymm_name(int config_index, int n_configs) {
  /** Convention for how a function for a chiral asymmetry is named.
      The name depends on the index of the specific chiral configuration and the
      total number of chiral configurations in this asymmetry.
  **/
  return "AsymmFactor" + std::to_string(config_index) + "_" +
         std::to_string(n_configs) + "allowed";
}

//------------------------------------------------------------------------------

} // Namespace Names
} // Namespace PrEWUtils