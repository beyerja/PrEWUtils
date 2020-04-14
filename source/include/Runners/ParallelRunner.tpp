#ifndef LIB_PARALLELRUNNER_TPP
#define LIB_PARALLELRUNNER_TPP 1

#include <Names/MinimizerNaming.h>
#include <Runners/ParallelRunner.h>

// Includes from PrEW
#include "Fit/ChiSqMinimizer.h"
#include "CppUtils/Str.h"

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace Runners {
  
//------------------------------------------------------------------------------

template <class SetupClass>
ParallelRunner<SetupClass>::ParallelRunner(
  const SetupClass & setup,
  const std::string & minimizers
) : 
  m_energies(setup.get_energies()),
  m_data_connector(setup.get_data_connector()),
  m_toy_gen(PREW::ToyMeas::ToyGen(m_data_connector,setup.get_pars()))
{
  /** Constructor extracts all relevant information from the setup.
      Minimizer string describes which Minuit2 minimizers to use.
      Multiple minimizers can be given separated by a "->".
      Allowed minimizers are:
        Migrad
        Simplex
        Combined
        Scan
        Fumili
        MigradBFGS
  **/
  // Extract the parameters which should be fitted for a given energy
  for (const auto & energy: m_energies) {
    m_pars[energy] = setup.get_pars(energy);
  }
  // Set up minimizers
  this->set_minimizers(minimizers);
} 

//------------------------------------------------------------------------------

template <class SetupClass>
PREW::Fit::ResultVec ParallelRunner<SetupClass>::run_toy_fits(
  int energy,
  int n_toys, 
  linx::ThreadPool * pool
) const {
  /** Run a given number of toy measurements at the given energy on a given 
      thread pool.
      Returns the corresponding fit results.
  **/
  
  // Check that energy is available
  if ( 
    std::find(m_energies.begin(),m_energies.end(),energy) == m_energies.end()
  ) {
    spdlog::error("ParallelRunner: Energy {} not available!", energy);
    return {};
  }
  
  PREW::Fit::ResultVec results ( n_toys );
  std::vector<std::future<PREW::Fit::FitResult>> result_futures ( n_toys );
  
  spdlog::debug("ParallelRunner: Start registering jobs for each toy @ E={}.", energy);
  for (size_t i=0; i<results.size(); i++) {
    // Create task with cpu-heavy work that can be executed on separate core
    spdlog::debug("ParallelRunner: Creating task {} @ E={}.", i, energy);
    auto task = [this,&energy](){ return this->single_fit_task(energy);  };
    // Queue the task -> ThreadPool will start it when resource available
    spdlog::debug("ParallelRunner: Queuing task {} @ E={}.", i, energy);
    result_futures[i] = pool->enqueue(task);
  }
  
  spdlog::debug("ParallelRunner: All threads registered @ E={}, waiting for them to return results.", energy);
  for ( size_t i=0; i<results.size(); i++ ) { 
    results[i] = result_futures[i].get();
  }
  
  return results; 
}

//------------------------------------------------------------------------------

template <class SetupClass>
PREW::Fit::ResultVec ParallelRunner<SetupClass>::run_toy_fits(
  int energy,
  int n_toys, 
  int n_threads
) const {
  /** Run a given number of toy measurements at the given energy on a given 
      number of threads.
      Returns the corresponding fit results.
  **/
  spdlog::debug("ParallelRunner: Creating thread pool for E={}.", energy);
  linx::ThreadPool pool(n_threads);
  return this->run_toy_fits( energy, n_toys, &pool );
}

//------------------------------------------------------------------------------

template <class SetupClass>
std::map<int,PREW::Fit::ResultVec> ParallelRunner<SetupClass>::run_toy_fits(
  int n_toys, 
  int n_threads
) const {
  /** Run a given number of toy measurements for all available energies on a 
      given number of threads.
      Returns the corresponding fit results for each energy.
  **/
  // Output maps energy to vector of fit results
  std::map<int,PREW::Fit::ResultVec> results_map {};
  
  spdlog::debug("ParallelRunner: Creating thread pool for all available energies.");
  linx::ThreadPool pool(n_threads);
  
  for (const auto & energy: m_energies) {
    results_map[energy] = 
      this->run_toy_fits( energy, n_toys, &pool ); 
  }
  
  spdlog::debug("ParallelRunner: Done with all energies!");
  return results_map;
}

//------------------------------------------------------------------------------

template <class SetupClass>
const PREW::Connect::DataConnector & 
ParallelRunner<SetupClass>::get_data_connector() const {
  return m_data_connector;
}

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

template <class SetupClass>
void ParallelRunner<SetupClass>::set_minimizers( 
  const std::string & minimizers_str
) {
  /** Decipher minimizer_str to figure out which minimizers are to be used and
      add their factories in order to the vector of minimizers.
      String must contain names of minimizers in order, separated by a "->"
      (representing the flow of results).
      Each minimizer can be assigned (MaxFcnCalls,MaxIters,Tolerance) options
      by writing them in such brackets after the name.
      Allowed minimizers are:
        Migrad
        Simplex
        Combined
        Scan
        Fumili
        MigradBFGS
      Generic example:
        "MinimizerID1(MaxFcnCalls,MaxIters,Tolerance)->MinimizerID1(MaxFcnCalls,MaxIters,Tolerance)->..."
  **/
  auto min_infos = Names::MinimizerNaming::read_mininimizer_str(minimizers_str);
  
  for (const auto & min_info : min_infos) {
    m_minuit_factories.push_back(
      PREW::Fit::MinuitFactory(
        min_info.m_type, 
        min_info.m_max_fcn_calls, 
        min_info.m_max_iters, 
        min_info.m_tolerance
      )
    );
  }
}

//------------------------------------------------------------------------------

template <class SetupClass>
PREW::Fit::FitResult 
ParallelRunner<SetupClass>::single_fit_task(int energy) const {
  /** Single complete toy fit task.
      Creates a poisson fluctuated toy measurement, sets up the fit container,
      performs the actual fit and returns its result.
  **/
  spdlog::debug("ParallelRunner: Create toy measurement @ E={}.", energy);
  auto distrs = m_toy_gen.get_fluctuated_distrs(energy);
  
  spdlog::debug("ParallelRunner: Set up fit container @ E={}.", energy);
  PREW::Fit::FitContainer container {};
  m_data_connector.fill_fit_container(
    distrs,
    m_pars.at(energy),
    &container
  );
  
  // Minimize with all given minimizers, save only the results of the last one
  PREW::Fit::FitResult final_result {};
  for (const auto & minuit_factory: m_minuit_factories) {
    final_result = this->single_minimization(&container, minuit_factory);
  }

  spdlog::info("ParallelRunner: Single minimization @ E={} finished.", energy);
  return final_result;
}

//------------------------------------------------------------------------------

template <class SetupClass>
PREW::Fit::FitResult ParallelRunner<SetupClass>::single_minimization(
  PREW::Fit::FitContainer * container_ptr, 
  const PREW::Fit::MinuitFactory & minuit_factory
) const {
  /** Perform chi^2 minimization on a given fit container with the given
      Minuit2 minimizer.
  **/
  spdlog::debug("ParallelRunner: Create minimizer.");
  PREW::Fit::ChiSqMinimizer chi_sq_minimizer (container_ptr, minuit_factory);
  
  spdlog::debug("ParallelRunner: Start minimization.");
  chi_sq_minimizer.minimize();
  
  spdlog::debug("ParallelRunner: Minimization finished.");
  return chi_sq_minimizer.get_result();
}

//------------------------------------------------------------------------------
  
} // Namespace Runners
} // Namespace PrEWUtils

#endif