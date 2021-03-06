#ifndef LIB_PARALLELRUNNER_H
#define LIB_PARALLELRUNNER_H 1

#include <DataHelp/BinSelector.h>
#include <Parallel/ThreadPool.h>
#include <Setups/FitModifier.h>

// Includes from PrEW
#include "Connect/DataConnector.h"
#include "Fit/FitResult.h"
#include "Fit/FitContainer.h"
#include "Fit/MinuitFactory.h"
#include "ToyMeas/ToyGen.h"

#include <map>
#include <vector>

namespace PrEWUtils {
namespace Runners {
  
  template <class SetupClass>
  class ParallelRunner {
    /** Class to run a given toy setup in multiple threads in parallel.
    **/
    
    std::vector<int> m_energies;
    std::map<int, PrEW::Fit::ParVec> m_pars; // Parameters used at each energy
    PrEW::Connect::DataConnector m_data_connector;
    PrEW::ToyMeas::ToyGen m_toy_gen;
    std::vector<PrEW::Fit::MinuitFactory> m_minuit_factories;
    std::string m_prew_minimizer;
    
    // Extra options
    bool m_use_selector {false};
    DataHelp::BinSelector m_bin_selector {};
    
    public:
      // Constructors
      ParallelRunner(
        const SetupClass & setup,
        const std::string & minuit_minimizers,
        const std::string & prew_minimizer
      );
      
      // Set extra options
      void set_bin_selector(DataHelp::BinSelector bin_selector);
      void modify_fit(const Setups::FitModifier &modifier);
      
      // Running toy fits
      PrEW::Fit::ResultVec run_toy_fits(
        int energy,
        int n_toys, 
        linx::ThreadPool * pool 
      ) const;
      
      PrEW::Fit::ResultVec run_toy_fits(
        int energy,
        int n_toys, 
        int n_threads
      ) const;
      
      std::map<int,PrEW::Fit::ResultVec> run_toy_fits(
        int n_toys, 
        int n_threads
      ) const;
      
      // Get info about current setup
      const PrEW::Connect::DataConnector & get_data_connector() const;

    protected:
      // Internal functions
      void set_minimizers( const std::string & minimizers_str );
      
      PrEW::Fit::FitResult single_fit_task(int energy) const;
      
      PrEW::Fit::FitResult single_minimization(
        PrEW::Fit::FitContainer * container_ptr, 
        const PrEW::Fit::MinuitFactory & minuit_factory
      ) const;
      template<class MinimizerClass> PrEW::Fit::FitResult single_minimization(
        MinimizerClass * Minimizer
      ) const;

  };
  
} // Namespace Runners
} // Namespace PrEWUtils

// Since the above is a template class the function definition has to happen
// either in the header itself or in a template file in the include directory
#include <Runners/ParallelRunner.tpp>

#endif