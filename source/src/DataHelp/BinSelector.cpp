#include <DataHelp/BinSelector.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace DataHelp {

//------------------------------------------------------------------------------
// Constructors

BinSelector::BinSelector( double cut_val, PREW::Fit::ParVec pars_for_cut ) :
  m_cut_val(cut_val), m_pars_for_cut(pars_for_cut) {}

//------------------------------------------------------------------------------

void BinSelector::remove_bins( PREW::Fit::FitContainer * container ) const {
  /** Function manipulates FitContainer, it removes all bins whose prediction
      is below the cutoff value for the set of parameters chosen for the cutoff.
      Preserves the parameters of the fitcontainer.
  **/
  int n_pars = container->m_fit_pars.size();
  
  // Preserve the initial parameters
  std::vector<double> par_vals_ini (n_pars);
  for ( int p=0; p<n_pars; p++ ) {
    par_vals_ini[p] = container->m_fit_pars[p].m_val_mod;
  }
  
  // Set the parameters to the set given at construction
  for ( const auto & par : m_pars_for_cut ) {
    // Look for this parameter and replace the value when found.
    // (Go old fashioned in this loop to be sure you know what's happening)
    for ( int p=0; p<n_pars; p++ ) {
      if ( container->m_fit_pars[p].get_name() == par.get_name() ) {
        container->m_fit_pars[p].m_val_mod = par.m_val_mod;
        break;
      }
    }
  }
  
  // Perform the cutoff:
  // First check which bins need to be cut, then cut them out of bin vector
  int n_bins = container->m_fit_bins.size();
  std::vector<int> ind_to_remove {}; // Vector for indices to remove
  for ( int b=0; b<n_bins; b++ ) {
    if ( container->m_fit_bins[b].get_val_prd() < m_cut_val ) {
      // Bin should be removed, save its index
      ind_to_remove.push_back(b);
    }
  }
  
  // Cut them from back to front to avoid index shift before cut
  for ( int i=ind_to_remove.size()-1; i>=0; i-- ) {
    int b = ind_to_remove[i];
    // Deleting the b-th element
    container->m_fit_bins.erase( container->m_fit_bins.begin() + b ); 
  }
  
  // Now reset the parameter values to how they were before bin erasure
  for ( int p=0; p<n_pars; p++ ) {
    container->m_fit_pars[p].m_val_mod = par_vals_ini[p];
  }
}

//------------------------------------------------------------------------------

} // Namespace DataHelp
} // Namespace PrEWUtils