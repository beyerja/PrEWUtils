#ifndef LIB_INPUTHELP_H
#define LIB_INPUTHELP_H 1

#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {
namespace InputHelp {
/** Namespace for input related helper functions.
 **/

std::vector<std::string> regex_search(const std::string &dir,
                                      const std::string &regex_name);

} // namespace InputHelp
} // namespace SetupHelp
} // namespace PrEWUtils

#endif