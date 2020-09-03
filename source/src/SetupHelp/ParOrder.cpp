#include <SetupHelp/ParOrder.h>

// includes from PrEW
#include <Data/FctLink.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------

bool ParOrder::par_fits_ID(const PrEW::Fit::FitPar &par,
                           const ParOrder::IDVec &ids) {
  /** Check if the parameter name fits any of the given IDs.
   **/
  bool fits_ID = false;
  for (const auto &id : ids) {
    auto par_name = par.get_name();
    // Check if id substring is in parameter name
    if ((par_name.find(id) != std::string::npos) || (par_name == id)) {
      // Found the ID in the name
      fits_ID = true;
    }
  }
  return fits_ID;
}

//------------------------------------------------------------------------------

ParOrder::CategorizedPars
ParOrder::categorize_pars(const PrEW::Fit::ParVec &pars,
                          const ParOrder::IDMap &id_map) {
  /** Categorize the given parameter set according to the ID map.
   **/
  CategorizedPars categorized_pars{};

  // First create empty vectors for all categories
  for (auto const &[key, _] : id_map) {
    categorized_pars[key] = {};
  }

  // Fill the categories
  for (const auto &par : pars) {
    bool found_id = false;
    for (auto const &[key, ids] : id_map) {
      if (ParOrder::par_fits_ID(par, ids)) {
        found_id = true;
        categorized_pars[key].push_back(par); // Add to map
        break; // Found parameter category, next parameter
      }
    }
    if (!found_id) {
      // Didn't find parameter category, something went wrong!
      throw std::invalid_argument("Can't find category for parameter: " +
                                  par.get_name());
    }
  }

  return categorized_pars;
}

//------------------------------------------------------------------------------

PrEW::Fit::ParVec ParOrder::reorder_pars(const PrEW::Fit::ParVec &pars,
                                         const ParOrder::Ordering &ordering,
                                         const ParOrder::IDMap &id_map) {
  /** Reorder the parameters according to the given ordering.
   **/
  PrEW::Fit::ParVec ordered_pars{};

  // Categorize the parameters
  auto categorized_pars = ParOrder::categorize_pars(pars, id_map);

  // Order them
  for (const auto &category : ordering) {
    auto category_pars = categorized_pars.at(category);
    spdlog::debug("Found {} pars in category {}.", category_pars.size(),
                  category);

    ordered_pars.insert(ordered_pars.end(), category_pars.begin(),
                        category_pars.end());
  }

  return ordered_pars;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils