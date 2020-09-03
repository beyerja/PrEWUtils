#ifndef LIB_DISTRHELP_TPP
#define LIB_DISTRHELP_TPP 1

#include <DataHelp/DistrHelp.h>

// Standard library
#include <algorithm>
#include <set>

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------

template <class T>
std::vector<std::string>
DistrHelp::find_distr_names(const std::vector<T> &distrs) {
  /** Find the list of all the distribution names contained in the informations
      of the given PrEW distributions.
   **/
  std::set<std::string> names{}; // Use set to avoid duplicates
  for (const auto &distr : distrs) {
    names.insert(distr.get_info().m_distr_name);
  }
  // Convert set to vector and return
  return std::vector<std::string>(names.begin(), names.end());
}

//------------------------------------------------------------------------------

// std::vector<std::string>
// DistrHelp::find_distr_names(const PrEW::Data::InfoVec &infos) {
//   /** Find the list of all the distribution names contained in the informations.
//    **/
//   std::set<std::string> names{}; // Use set to avoid duplicates
//   for (const auto &info : infos) {
//     names.insert(info.m_distr_name);
//   }
//   // Convert set to vector and return
//   return std::vector<std::string>(names.begin(), names.end());
// }

//------------------------------------------------------------------------------

template <class T>
const PrEW::Data::InfoVec DistrHelp::find_infos(const std::vector<T> &distrs) {
  /** Find all infos in a given distribution vector.
   **/
   PrEW::Data::InfoVec infos{};
   for (const auto &distr : distrs) {
     infos.push_back(distr.get_info());
   }
   return infos;
}

//------------------------------------------------------------------------------

template <class T>
typename std::vector<T>::iterator
DistrHelp::find_info(std::vector<T> &distrs,
                     const PrEW::Data::DistrInfo &info) {
  /** Perform a find on the given vector and try to find a match for the info.
   **/
  auto cond = [info](const T &distr) { return distr.get_info() == info; };
  return std::find_if(distrs.begin(), distrs.end(), cond);
}

//------------------------------------------------------------------------------

} // namespace DataHelp
} // namespace PrEWUtils

#endif