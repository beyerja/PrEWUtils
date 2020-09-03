#ifndef LIB_DISTRHELP_H
#define LIB_DISTRHELP_H 1

// includes from PrEW
#include <Data/DistrInfo.h>

// Standard library
#include <set>
#include <string>
#include <vector>

namespace PrEWUtils {
namespace DataHelp {

namespace DistrHelp {
/** Namespace for helper functions relating to the PrEW distribution classes.
 **/

template <class T>
std::vector<std::string> find_distr_names(const std::vector<T> &distrs);
std::vector<std::string> find_distr_names(const PrEW::Data::InfoVec &infos);

template <class T>
const PrEW::Data::InfoVec find_infos(const std::vector<T> &distrs);

template <class T>
typename std::vector<T>::iterator find_info(std::vector<T> &distrs,
                                            const PrEW::Data::DistrInfo &info);

} // namespace DistrHelp

} // namespace DataHelp
} // namespace PrEWUtils

// The above contains template function whose definition has to happen either in
// the header itself or in a template file in the include directory
#include <DataHelp/DistrHelp.tpp>
#endif