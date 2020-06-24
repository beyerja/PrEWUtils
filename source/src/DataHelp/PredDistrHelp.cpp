#include <DataHelp/PredDistrHelp.h>
#include <Names/CoefNaming.h>

// Includes from PrEW
#include <Data/CoefDistr.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------

double PredDistrHelp::get_pred_sum (
  const PrEW::Data::PredDistr & pred, 
  const std::string & type
) {
  /** Return the sum of the predicted distribution bins.
      Can be "signal", "background" or "S+B".
  **/
  double sum = 0;
  
  if ( ( type == "signal" ) || ( type == "S+B" ) ) {
    for (const auto & val: pred.m_sig_distr) { sum += val; }
  } else if ( ( type == "background" ) || ( type == "S+B" ) ) {
    for (const auto & val: pred.m_bkg_distr) { sum += val; }
  } else {
    spdlog::warn("PredDistrHelp: Unknown sum type: {}", type);
  }
  
  return sum;
}

//------------------------------------------------------------------------------

std::vector<double>
PredDistrHelp::pred_to_coef(const PrEW::Data::PredDistr &pred,
                            const std::string &type) {
  /** Turn the given part of the distribution (type=signal/background/total) 
      into a differential coefficient.
   **/
  if ((type != "signal") && (type != "background") && (type != "total")) {
    throw std::invalid_argument(("Unknown type of distr coef requested: "
                                + type).c_str());
  }
   
  int n_bins = pred.m_bin_centers.size();
   
  std::vector<double> coefs (n_bins, 0);
  
  // Get the coefficient depending on the requested type
  for (int b=0; b<n_bins; b++) {
    if ( (type == "signal") || (type == "total") ) {
      coefs[b] += pred.m_sig_distr[b];
    }
    if ( (type == "background") || (type == "total") ) {
      coefs[b] += pred.m_bkg_distr[b];
    }
  }
  
  return coefs;
}

//------------------------------------------------------------------------------

}
}