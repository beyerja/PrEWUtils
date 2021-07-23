#ifndef LIB_PARALLELRUNNER_TPP
#define LIB_PARALLELRUNNER_TPP 1

#include <Names/MinimizerNaming.h>
#include <Runners/ParallelRunner.h>

// Includes from PrEW
#include "CppUtils/Str.h"
#include "Fit/ChiSqMinimizer.h"
#include "Fit/PoissonNLLMinimizer.h"
#include "ToyMeas/ParFlct.h"

// External libraries
#include "spdlog/spdlog.h"

// Ignore non-crucial warnings from the external library
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <chrono>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <thread>
#pragma GCC diagnostic pop

namespace PrEWUtils {
namespace Runners {

//------------------------------------------------------------------------------

template <class SetupClass>
ParallelRunner<SetupClass>::ParallelRunner(const SetupClass &setup,
                                           const std::string &minuit_minimizers,
                                           const std::string &prew_minimizer)
    : m_energies(setup.get_energies()),
      m_data_connector(setup.get_data_connector()),
      m_toy_gen(PrEW::ToyMeas::ToyGen(m_data_connector, setup.get_pars())),
      m_prew_minimizer(prew_minimizer) {
  /** Constructor extracts all relevant information from the setup.
      Minuit/PrEW minimizer string describes which Minuit2/PrEW minimizers to
      use.
      Allowed PrEW minimizers are:
        ChiSquared
        PoissonNLL
      Allowed Minuit2 minimizers are:
        Migrad
        Simplex
        Combined
        Scan
        Fumili
        MigradBFGS
      Multiple Minuit2 minimizers can be given separated by a "->".
  **/
  // Extract the parameters which should be fitted for a given energy
  for (const auto &energy : m_energies) {
    m_pars[energy] = setup.get_pars(energy);
  }
  // Set up Minuit2 minimizers
  this->set_minimizers(minuit_minimizers);
}

//------------------------------------------------------------------------------

template <class SetupClass>
void ParallelRunner<SetupClass>::set_bin_selector(
    DataHelp::BinSelector bin_selector) {
  /** Set a bin selector that excludes bins from the minimization process.
   **/
  m_bin_selector = bin_selector;
  m_use_selector = true;
}

//------------------------------------------------------------------------------

template <class SetupClass>
void ParallelRunner<SetupClass>::modify_fit(
    const Setups::FitModifier &modifier) {
  /** Add a modifier that changes the fit to the toy measurements.
      Notice that the toy measurements themselves will not be affected.
   **/
  modifier.modify_setup(&m_data_connector, &(m_pars[modifier.get_energy()]));
}

//------------------------------------------------------------------------------

template <class SetupClass>
PrEW::Fit::ResultVec
ParallelRunner<SetupClass>::run_toy_fits(int energy, int n_toys,
                                         linx::ThreadPool *pool) const {
  /** Run a given number of toy measurements at the given energy on a given
      thread pool.
      Returns the corresponding fit results.
  **/

  // Check that energy is available
  if (std::find(m_energies.begin(), m_energies.end(), energy) ==
      m_energies.end()) {
    spdlog::error("ParallelRunner: Energy {} not available!", energy);
    return {};
  }

  PrEW::Fit::ResultVec results(n_toys);
  std::vector<std::future<PrEW::Fit::FitResult>> result_futures(n_toys);

  spdlog::debug("ParallelRunner: Start registering jobs for each toy @ E={}.",
                energy);
  for (size_t i = 0; i < results.size(); i++) {
    // Create task with cpu-heavy work that can be executed on separate core
    spdlog::debug("ParallelRunner: Creating task {} @ E={}.", i, energy);
    auto task = [this, &energy]() { return this->single_fit_task(energy); };
    // Queue the task -> ThreadPool will start it when resource available
    spdlog::debug("ParallelRunner: Queuing task {} @ E={}.", i, energy);
    result_futures[i] = pool->enqueue(task);
  }

  spdlog::debug("ParallelRunner: All threads registered @ E={}, waiting for "
                "them to return results.",
                energy);

  // TODO
  // COMMENTS
  // TODO

  auto n_results = results.size();
  using namespace indicators::option;
  indicators::ProgressBar bar{BarWidth{30}, PrefixText{"Fits finished: "},
                              ShowElapsedTime{true}, MaxProgress{n_results}};
  for (size_t i = 0; i < n_results; i++) {
    results[i] = result_futures[i].get();
    bar.set_option(
        PostfixText{std::to_string(i + 1) + "/" + std::to_string(n_results)});
    bar.tick();
  }

  return results;
}

//------------------------------------------------------------------------------

template <class SetupClass>
PrEW::Fit::ResultVec
ParallelRunner<SetupClass>::run_toy_fits(int energy, int n_toys,
                                         int n_threads) const {
  /** Run a given number of toy measurements at the given energy on a given
      number of threads.
      Returns the corresponding fit results.
  **/
  spdlog::debug("ParallelRunner: Creating thread pool for E={}.", energy);
  linx::ThreadPool pool(n_threads);
  return this->run_toy_fits(energy, n_toys, &pool);
}

//------------------------------------------------------------------------------

template <class SetupClass>
std::map<int, PrEW::Fit::ResultVec>
ParallelRunner<SetupClass>::run_toy_fits(int n_toys, int n_threads) const {
  /** Run a given number of toy measurements for all available energies on a
      given number of threads.
      Returns the corresponding fit results for each energy.
  **/
  // Output maps energy to vector of fit results
  std::map<int, PrEW::Fit::ResultVec> results_map{};

  spdlog::debug(
      "ParallelRunner: Creating thread pool for all available energies.");
  linx::ThreadPool pool(n_threads);

  for (const auto &energy : m_energies) {
    results_map[energy] = this->run_toy_fits(energy, n_toys, &pool);
  }

  spdlog::debug("ParallelRunner: Done with all energies!");
  return results_map;
}

//------------------------------------------------------------------------------

template <class SetupClass>
const PrEW::Connect::DataConnector &
ParallelRunner<SetupClass>::get_data_connector() const {
  return m_data_connector;
}

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

template <class SetupClass>
void ParallelRunner<SetupClass>::set_minimizers(
    const std::string &minimizers_str) {
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

  for (const auto &min_info : min_infos) {
    m_minuit_factories.push_back(
        PrEW::Fit::MinuitFactory(min_info.m_type, min_info.m_max_fcn_calls,
                                 min_info.m_max_iters, min_info.m_tolerance));
  }
}

//------------------------------------------------------------------------------

template <class SetupClass>
PrEW::Fit::FitResult
ParallelRunner<SetupClass>::single_fit_task(int energy) const {
  /** Single complete toy fit task.
      Creates a poisson fluctuated toy measurement, sets up the fit container,
      performs the actual fit and returns its result.
  **/
  spdlog::debug("ParallelRunner: Create toy measurement @ E={}.", energy);
  auto distrs = m_toy_gen.get_fluctuated_distrs(energy);
  auto pars = m_pars.at(energy);
  PrEW::ToyMeas::ParFlct::fluctuate_constrs(pars);

  spdlog::debug("ParallelRunner: Set up fit container @ E={}.", energy);
  PrEW::Fit::FitContainer container{};
  m_data_connector.fill_fit_container(distrs, pars, &container);

  // If requested remove bins according to selector
  if (m_use_selector) {
    m_bin_selector.remove_bins(&container);
  }

  // Minimize with all given minimizers, save only the results of the last one
  PrEW::Fit::FitResult final_result{};
  for (const auto &minuit_factory : m_minuit_factories) {
    final_result = this->single_minimization(&container, minuit_factory);
  }

  spdlog::debug("ParallelRunner: Single minimization @ E={} finished.", energy);
  return final_result;
}

//------------------------------------------------------------------------------

template <class SetupClass>
PrEW::Fit::FitResult ParallelRunner<SetupClass>::single_minimization(
    PrEW::Fit::FitContainer *container_ptr,
    const PrEW::Fit::MinuitFactory &minuit_factory) const {
  /** Start a minimisation on the given fit container with the given Minuit2
      minimizer and the PrEW minimizer that was requested at initialisation.
      Return the result.
  **/

  spdlog::debug("ParallelRunner: Create minimizer: {}.", m_prew_minimizer);
  if (m_prew_minimizer == "ChiSquared") {
    auto minimizer = PrEW::Fit::ChiSqMinimizer(container_ptr, minuit_factory);
    return this->single_minimization(&minimizer);
  } else if (m_prew_minimizer == "PoissonNLL") {
    auto minimizer =
        PrEW::Fit::PoissonNLLMinimizer(container_ptr, minuit_factory);
    return this->single_minimization(&minimizer);
  } else {
    throw std::invalid_argument(
        ("Unknown PrEW minimizer type " + m_prew_minimizer).c_str());
  }
}

//------------------------------------------------------------------------------

template <class SetupClass>
template <class MinimizerClass>
PrEW::Fit::FitResult ParallelRunner<SetupClass>::single_minimization(
    MinimizerClass *minimizer) const {
  /** Perform a minimisation task on the given minimizer and return the result.
   **/

  spdlog::debug("ParallelRunner: Start minimization.");
  minimizer->minimize();

  spdlog::debug("ParallelRunner: Minimization finished.");
  return minimizer->get_result();
}

//------------------------------------------------------------------------------

} // Namespace Runners
} // Namespace PrEWUtils

#endif