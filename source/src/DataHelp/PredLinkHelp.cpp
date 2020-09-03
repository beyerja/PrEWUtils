#include <DataHelp/DistrHelp.h>
#include <DataHelp/PredLinkHelp.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------

void PredLinkHelp::add_link_to_vec(const PrEW::Data::PredLink &link,
                                   PrEW::Data::PredLinkVec &vec) {
  /** Add a given prediction link to the given vector if it is not already
      contained in the vector.
   **/
   
   auto link_it =
       DataHelp::DistrHelp::find_info(vec, link.get_info());

   if (link_it == vec.end()) {
     // No link for this distribution, set as new
     vec.push_back(link);
   } else {
     // Link already exists, add new info to already existing link
     auto *sig_links = &link_it->m_fcts_links_sig;
     auto *bkg_links = &link_it->m_fcts_links_bkg;
     auto new_sig_links = link.m_fcts_links_sig;
     auto new_bkg_links = link.m_fcts_links_bkg;

     sig_links->insert(sig_links->end(), new_sig_links.begin(),
                       new_sig_links.end());
     bkg_links->insert(bkg_links->end(), new_bkg_links.begin(),
                       new_bkg_links.end());
   }
}

void PredLinkHelp::add_links_to_vec(const PrEW::Data::PredLinkVec &links,
                                    PrEW::Data::PredLinkVec &vec) {
  /** Add the given prediction link to the given vector if they are not already
      contained in the vector.
   **/
  for (const auto &link : links) {
    add_link_to_vec(link, vec);
  }
}

//------------------------------------------------------------------------------

} // namespace DataHelp
} // namespace PrEWUtils