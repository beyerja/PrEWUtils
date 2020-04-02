#include <DataHelp/PredDistrHelp.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------

double PredDistrHelp::get_pred_sum (
  const PREW::Data::PredDistr & pred, 
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

}
}