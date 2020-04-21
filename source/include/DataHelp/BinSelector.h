#ifndef LIB_BINSELECTOR_H
#define LIB_BINSELECTOR_H 1

// Includes from PrEW
#include "Fit/FitContainer.h"
#include "Fit/FitPar.h"

// TODO TODO TODO This should be part of PrEW
namespace PrEWUtils {
namespace DataHelp {
  
  class BinSelector {
    /** Class designed to execute a cutoff on which bins should be considered
        in a minimization.
        The cutoff checks the bin prediction for a given set of parameter values
        and if it is below a given cutoff the bin is not considered in the 
        minimization.
    **/

    double m_cut_val {}; 
    PREW::Fit::ParVec m_pars_for_cut {};

    public:
      // Constructors
      BinSelector() {};
      BinSelector(double cut_val, PREW::Fit::ParVec pars_for_cut);
      
      // Core functionality
      void remove_bins(PREW::Fit::FitContainer * container) const;
  };
  
} // Namespace DataHelp
} // Namespace PrEWUtils

#endif