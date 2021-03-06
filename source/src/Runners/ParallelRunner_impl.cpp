#include <Runners/ParallelRunner.h>
#include <Setups/GeneralSetup.h>

namespace PrEWUtils {
namespace Runners {
  /** Instantiating ParallelRunner with each possible setup.
      Serves the sole purpose of compiling the header template in order to 
      figure out if code is correct.
      Note meant to be included or used anywhere.
  **/
  
  template class ParallelRunner<Setups::GeneralSetup>;
  
} // Namespace Runners
} // Namespace PrEWUtils