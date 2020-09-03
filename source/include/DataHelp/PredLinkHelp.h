#ifndef LIB_PREDLINKHELP_H
#define LIB_PREDLINKHELP_H 1

// includes from PrEW
#include <Data/PredLink.h>

namespace PrEWUtils {
namespace DataHelp {
  
namespace PredLinkHelp {
/** Helper functions for anything related to PrEW prediction links.
 **/

void add_link_to_vec(const PrEW::Data::PredLink &coef,
                     PrEW::Data::PredLinkVec &vec);
void add_links_to_vec(const PrEW::Data::PredLinkVec &coefs,
                      PrEW::Data::PredLinkVec &vec);

} // namespace PredLinkHelp

} // namespace DataHelp
} // namespace PrEWUtils

#endif