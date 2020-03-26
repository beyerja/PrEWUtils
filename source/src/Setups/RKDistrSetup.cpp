#include <Names/CoefNaming.h>
#include <Names/ParNaming.h>
#include <Setups/RKDistrSetup.h>

// Includes from PrEW
#include <Data/DistrUtils.h>
#include <Data/FctLink.h>
#include <GlobalVar/Chiral.h>
#include <Input/InfoRKFile.h>
#include <Input/DataReader.h>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace Setups {

//------------------------------------------------------------------------------

RKDistrSetup::RKDistrSetup() {}

//------------------------------------------------------------------------------

RKDistrSetup::RKDistrSetup(const std::string & file_path, int energy)
{
  /** Constructor meant for when looking at only one energy and file.
      (Can also be used in general case.)
  **/
  this->add_input_file(file_path); // Read file
  this->add_energy(energy); // Use the given energy
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_input_file(const std::string & file_path)
{
  /** Add another file whose input is added to also taken into account.
  **/
  m_input_files.push_back(file_path);
}
  
//------------------------------------------------------------------------------

void RKDistrSetup::add_energy(int energy) {
  /** Select an energy for which the setup should be produced.
  **/
  m_energies.push_back(energy);
}

//------------------------------------------------------------------------------

void RKDistrSetup::use_distr(const std::string & distr_name) {
  /** Select a distribution to be used in the fit.
      Options:
       4f semileptonic:
        "singleWplussemileptonic" 
        "singleWminussemileptonic"
        "WWsemileptonic" 
        "ZZsemileptonic"
      4f other:
        "singleWplusleptonic"
        "singleWminusleptonic"
        "WWhadronic"
        "WWleptonic"
        "ZZhadronic"
        "ZZleptonic"
        "ZZWWMixhadronic"
        "ZZWWMixleptonic"
      2f: 
        "Zhadronic"
        "Zleptonic"
    From additional files:
      "WW_semilep_MuAntiNu"
      "WW_semilep_AntiMuNu"
  **/
  m_used_distr_names.push_back(distr_name);
}

//------------------------------------------------------------------------------

void RKDistrSetup::set_lumi(
  int energy,
  double val,
  double ini_unc
) {
  /** Set the total luminosity (before splitting between pol configs).
  **/  
  std::string name = Names::ParNaming::lumi_name(energy);
  
  // Check if lumi for this energy already added
  auto name_cond = [name](const PREW::Fit::FitPar &par){return par.get_name()==name;};
  if ( 
    find_if(m_common_pars.begin(), m_common_pars.end(), name_cond) 
    != m_common_pars.end() 
  ) {
    spdlog::warn("Lumi for energy {} already set, not resetting", energy);
    return;
  }
  
  m_separate_pars[energy].push_back( {name, val, ini_unc} );
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_pol(
  const std::string & name,
  int energy,
  double val,
  double ini_unc
) {
  /** Add a single beam polarisation.
      Automatically sets restriction that pol can only be between -1 and +1.
  **/
  PREW::Fit::FitPar new_pol {name, val, ini_unc};
  new_pol.set_limits(-1,1);
  m_separate_pars[energy].push_back(new_pol);
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_lumi_constr( 
  int energy, 
  double constr_val, 
  double constr_unc
) {
  /** Add a gaussian constraint on the luminosity for the run at the given 
      energy.
  **/
  std::string name = Names::ParNaming::lumi_name(energy);
  auto name_cond = 
    [name](const PREW::Fit::FitPar &par){return par.get_name()==name;};
  auto lumi_it = 
    find_if( 
      m_separate_pars.at(energy).begin(), 
      m_separate_pars.at(energy).end(), 
      name_cond
    );
  lumi_it->set_constrgauss( {constr_val, constr_unc} );
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_pol_constr(
  const std::string & name,
  int energy,
  double constr_val,
  double constr_unc
) {
  /** Add a gaussian constraint on the given single beam polarisation.
  **/
  auto name_cond = 
    [name](const PREW::Fit::FitPar &par){return par.get_name()==name;};
  auto pol_it = 
    find_if( 
      m_separate_pars.at(energy).begin(), 
      m_separate_pars.at(energy).end(), 
      name_cond
    );
  pol_it->set_constrgauss( {constr_val, constr_unc} );
}


//------------------------------------------------------------------------------

void RKDistrSetup::add_pol_config(
  const std::string & name,
  int energy,
  const std::string & e_pol_name,
  const std::string & p_pol_name,
  const std::string & e_pol_sign,
  const std::string & p_pol_sign,
  double lumi_fraction
) {
  /** Add a polarisation configuration with a given luminosity fraction.
  **/
  m_pol_links.push_back(
    PREW::Data::PolLink(energy,name,e_pol_name,p_pol_name,e_pol_sign,p_pol_sign)
  );
  m_lumi_fractions[energy][name] = lumi_fraction;
}

//------------------------------------------------------------------------------

void RKDistrSetup::activate_cTGCs() { 
  /** Activate usage of anomalous cTGCs.
      Currently available: the three cTGCs in LEP parameterisation.
      All will be set to zero here.
  **/
  m_use_cTGCs = true;
  m_common_pars.push_back( {"Delta-g1Z", 0.0, 0.0001} );
  m_common_pars.push_back( {"Delta-kappa_gamma", 0.0, 0.0001} );
  m_common_pars.push_back( {"Delta-lambda_gamma", 0.0, 0.0001} );
}

//------------------------------------------------------------------------------

void RKDistrSetup::free_chiral_xsection( 
  const std::string & distr_name,
  const std::string & chiral_config 
) {
  /** Set a chiral cross section of a given distribution to be a free parameter
      in the fit.
      Will lead to an extra fit parameter which is a factor applied to each bin
      of the chiral differential distribution.
  **/  
  m_common_pars.push_back( 
    { Names::ParNaming::chi_xs_par_name(distr_name,chiral_config), 1.0, 0.0001 } 
  );
  m_free_xs_chi[distr_name].push_back(chiral_config);
}

//------------------------------------------------------------------------------
// Markers that should be set to get correct WW and ZZ cross sections

void RKDistrSetup::set_WW_mu_only() { m_WW_mu_only = true; }
void RKDistrSetup::set_ZZ_mu_only() { m_ZZ_mu_only = true; }

//------------------------------------------------------------------------------

void RKDistrSetup::complete_setup() {
  /** 
      Set up the function links and coefficients according to the configurations
      provided previously.
      Reads input files and creates all the complicated function linking 
      instructions that are needed to set up a proper running fit.
  **/
  
  this->read_input_files();
  
  for (int & energy : m_energies) {    
    for (const auto & distr_name : m_used_distr_names) {    
      this->complete_distr_setup( distr_name, energy );
    } 
  } 
}

//------------------------------------------------------------------------------

const std::vector<int> & RKDistrSetup::get_energies() const {return m_energies;}

//------------------------------------------------------------------------------

PREW::Connect::DataConnector RKDistrSetup::get_data_connector() const {
  /** Get the connector that contains all the information that it needs to 
      properly link the predicition functions.
      Must be called _after_ calling final combination.
  **/
  return PREW::Connect::DataConnector(
    m_used_distrs,
    m_used_coefs,
    m_pred_links,
    m_pol_links
  );
}

//------------------------------------------------------------------------------

PREW::Fit::ParVec RKDistrSetup::get_pars (int energy) const { 
  /** Get the parameters that are to be used by the prediciton functions at the
      given energy.
      Must be called _after_ calling final combination.
  **/
  PREW::Fit::ParVec pars = m_common_pars;
  pars.insert(
    pars.end(), 
    m_separate_pars.at(energy).begin(),
    m_separate_pars.at(energy).end()
  );
  return pars; 
}
  
//------------------------------------------------------------------------------

PREW::Fit::ParVec RKDistrSetup::get_pars () const { 
  /** Get all parameters that are to be used by the prediciton functions at any
      energy.
      Must be called _after_ calling final combination.
  **/
  PREW::Fit::ParVec pars = m_common_pars;
  for (const auto & [energy, separate_pars]: m_separate_pars) {
    pars.insert( pars.end(), separate_pars.begin(), separate_pars.end() );
  }
  return pars; 
}

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void RKDistrSetup::complete_distr_setup(
  const std::string & distr_name,
  int energy
) {
  /** Complete setting up the instructions, parameters and coefficicents for 
      the given distribution at the given energy.
  **/
  // Mark distribution as used and determine binning of the distributions
  auto distrs = 
    PREW::Data::DistrUtils::subvec_energy_and_name(
      m_input_distrs, energy, distr_name
    );
  m_used_distrs.insert( m_used_distrs.end(), distrs.begin(), distrs.end() );
  int n_bins = distrs.at(0).m_bin_centers.size();
  
  // Get coefficients for this distribution and mark them as used
  auto coefs = 
    PREW::Data::DistrUtils::subvec_energy_and_name( 
      m_input_coefs, energy, distr_name 
    );
  m_used_coefs.insert( m_used_coefs.end(), coefs.begin(), coefs.end() );
  
  // Create info classes for each pol. & chiral config. for this distribution
  auto infos_pol = this->get_infos_pol( distr_name, energy );

  // Set up all information for each chiral configuration
  for ( const auto & info_chi: this->get_infos_chi( distr_name, energy ) ) {
    this->complete_chi_setup( info_chi, n_bins );
  }
  
  // Set up all information for each polarisation configuration
  for ( const auto & info_pol: this->get_infos_pol( distr_name, energy ) ) {
    this->complete_pol_setup( info_pol, n_bins );
  }
}

//------------------------------------------------------------------------------

void RKDistrSetup::complete_chi_setup(
  const PREW::Data::DistrInfo & info_chi, 
  int n_bins
) {
  /** Set up the links, coefficients and parameters for a given chiral 
      distribution with a given number of bins.
  **/
  // Set up the function links for this chiral configuration
  PREW::Data::FctLinkVec sig_fct_links {};
  PREW::Data::FctLinkVec bkg_fct_links {};
  
  // Remove taus from WW if requested
  if ( m_WW_mu_only && (info_chi.m_distr_name == "WWsemileptonic") ) {
    this->add_tau_removal_coef( info_chi, n_bins );
    sig_fct_links.push_back( this->get_tau_removal_fct_link() );
  }
  
  // Remove taus and neutrinos form ZZ if requested
  if ( m_ZZ_mu_only && (info_chi.m_distr_name == "ZZsemileptonic") ) {
    this->add_nu_and_tau_removal_coef( info_chi, n_bins );
    sig_fct_links.push_back( this->get_nu_and_tau_removal_fct_link() );
  }

  // Use charged Triple Gauge Couplings in fit if requested & available
  if (  m_use_cTGCs && 
        this->has_cTGCs_available( info_chi.m_distr_name, info_chi.m_energy ) 
  ) {
    // Coefficient that is simply 1.0, needed for TGC parameterisation
    this->add_unity_coef( info_chi, n_bins);
    // Function that parametrises effect of cTGCs
    sig_fct_links.push_back( this->get_cTGC_fct_link() );
  }
  
  // If total chiral cross section is supposed to be free, add cross section
  // scaling parameter
  if ( this->xs_chi_is_free( info_chi ) ) {
    sig_fct_links.push_back( this->get_chi_xs_fct_link( info_chi ) );
  }
  
  PREW::Data::PredLink link_chi { info_chi, sig_fct_links, bkg_fct_links };
  m_pred_links.push_back(link_chi);
}

//------------------------------------------------------------------------------

void RKDistrSetup::complete_pol_setup(
  const PREW::Data::DistrInfo & info_pol, 
  int n_bins
) {
  /** Set up the links, coefficients and parameters for a given polarised 
      distribution with a given number of bins.
  **/
  // Set up the function links for this polarisation configuration
  PREW::Data::FctLinkVec sig_fct_links {};
  PREW::Data::FctLinkVec bkg_fct_links {};
    
  // Add instructions for luminosity factor
  this->add_lumi_fraction_coef(info_pol, n_bins);
  sig_fct_links.push_back( this->get_lumi_fraction_fct_link( info_pol ) );
    
  PREW::Data::PredLink link_pol { info_pol, sig_fct_links, bkg_fct_links };
  m_pred_links.push_back(link_pol);
}

//------------------------------------------------------------------------------

void RKDistrSetup::read_input_file(const std::string & file_path, int energy) {
  /** Read input from a given file at a given energy.
  **/
  // PrEW interpretable input file info 
  PREW::Input::InfoRKFile i {file_path, "RK", energy};
  // File reading
  PREW::Input::DataReader reader(&i);
  reader.read_file();
  auto new_pred = reader.get_pred_distrs();
  auto new_coef = reader.get_coef_distrs();
  // Adding results to input vectors for later use
  m_input_distrs.insert(m_input_distrs.end(), new_pred.begin(), new_pred.end());
  m_input_coefs.insert(m_input_coefs.end(), new_coef.begin(), new_coef.end());
}

//------------------------------------------------------------------------------

void RKDistrSetup::read_input_files() {
  /** Read input from all available files at all previously provided energies.
  **/
  for (const auto & file_path: m_input_files) {
    for (const auto & energy: m_energies) {
      this->read_input_file(file_path, energy);
    }
  }
}

//------------------------------------------------------------------------------

bool RKDistrSetup::has_cTGCs_available(
  const std::string & distr_name,
  int energy 
) const {
  /** Check the available coefficients if the coefficients which are needed 
      for the cTGC parameterisation are available for the given distribution
      at the given energy. 
      (Checks only for availability of first coefficient.)
  **/
  bool cTGCs_available = false;
  
  // Find coefficients for this distribution to check for cTGC coefs
  auto coefs = 
    PREW::Data::DistrUtils::subvec_energy_and_name( 
      m_used_coefs, energy, distr_name
    );
  
  // Look if coefs are available
  for ( const auto & coef : coefs) {
    if ( coef.m_coef_name == "TGCA" ) {
      cTGCs_available = true;
      break;
    }
  }
  return cTGCs_available;
}

//------------------------------------------------------------------------------

bool RKDistrSetup::xs_chi_is_free(
  const PREW::Data::DistrInfo & info_chi
) const {
  /** Check if the chiral cross section of the given distribution is supposed
      to be a free parameter.
  **/
  bool free = false;
  // Find all free chiral xs for this distribution
  const auto free_chi_xs_it = m_free_xs_chi.find(info_chi.m_distr_name);
  if ( free_chi_xs_it != m_free_xs_chi.end() ) {
    const auto & free_chi_xs = (*free_chi_xs_it).second;
    
    // See if the given chiral state is supposed to be free
    if (
      std::find(free_chi_xs.begin(), free_chi_xs.end(), info_chi.m_pol_config) 
      != free_chi_xs.end()
    ) {
      // Found this chiral config => Is supposed to be free    
      free = true;
    }
  }
  
  return free;
}

//------------------------------------------------------------------------------

std::vector<PREW::Data::DistrInfo> RKDistrSetup::get_infos_pol(
  const std::string & distr_name,
  int energy
) const {
  /** Create distribution info containers for the given distribution for all 
      available polarisation configurations at the given energy.
  **/
  std::vector<PREW::Data::DistrInfo> infos_pol {};
  for ( const auto & pol_link : m_pol_links ) {
    if ( pol_link.get_energy() == energy ) {
      infos_pol.push_back( { distr_name, pol_link.get_pol_config(), energy } );
    }
  }
  return infos_pol;
}

//------------------------------------------------------------------------------

std::vector<PREW::Data::DistrInfo> RKDistrSetup::get_infos_chi(
  const std::string & distr_name,
  int energy
) const {
  /** Create distribution info containers for the given distribution for all 
      chiral configurations at the given energy.
  **/
  std::vector<PREW::Data::DistrInfo> infos_chi {
    {distr_name, PREW::GlobalVar::Chiral::eLpR, energy},
    {distr_name, PREW::GlobalVar::Chiral::eRpL, energy},
    {distr_name, PREW::GlobalVar::Chiral::eLpL, energy},
    {distr_name, PREW::GlobalVar::Chiral::eRpR, energy}
  };
  return infos_chi;
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_unity_coef( 
  const PREW::Data::DistrInfo & info,
  int n_bins
) {
  /** Add a coefficient which is simply 1.0 for each bin of the distribution 
      described by the given info.
  **/
  // TODO ADD CONVENTION FOR "ONE" PARAMETER TO NAMES NAMESPACE
  m_used_coefs.push_back({"One", info, std::vector<double>(n_bins,1.0)});
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_tau_removal_coef(
  const PREW::Data::DistrInfo & info,
  int n_bins
) {
  /** Add the coefficient that is needed to remove taus from the muon/tau 
      mixture of W decays.
  **/
  // TODO COEFFICIENT CONVENTIONS IN NAMES NAMESPACE!!!
  double tau_removal = 0.5; // Remove tau from tau/muon mixture
  m_used_coefs.push_back(
    {"TauRemovalFactor", info, std::vector<double>(n_bins,tau_removal)}
  );
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_nu_and_tau_removal_coef(
  const PREW::Data::DistrInfo & info,
  int n_bins
) {
  /** Add the coefficient that is needed to remove neutrinos (2. & 3. gen) and 
      taus from mixture of 2. & 3. gen. lepton decays of Z bosons.
  **/
  // TODO COEFFICIENT CONVENTIONS IN NAMES NAMESPACE!!!
  double nu_and_tau_removal = 0.168;
  m_used_coefs.push_back(
    {
      "NuAndTauRemovalFactor", 
      info, 
      std::vector<double>(n_bins,nu_and_tau_removal)
    }
  );
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_lumi_fraction_coef(
  const PREW::Data::DistrInfo & info_pol,
  int n_bins
) {
  /** Add the coefficient for the luminosity fraction of a given distribution 
      (for each bin).
  **/
  m_used_coefs.push_back(
    { Names::CoefNaming::lumi_fraction_name(info_pol), 
      info_pol,
      std::vector<double>( 
        n_bins, 
        m_lumi_fractions.at(info_pol.m_energy).at(info_pol.m_pol_config) 
      )
    }
  );
}

//------------------------------------------------------------------------------

// TODO THIS IN ITS OWN NAMESPACE
PREW::Data::FctLink RKDistrSetup::get_cTGC_fct_link() const {
  /** Returns instruction that describes how cTGC parameterisation is built.
  **/
  PREW::Data::FctLink cTGC_fct_link {
    "Quadratic3DPolynomial_Coeff", // Name of function to use
    { // Names of parameters to use 
      "Delta-g1Z",
      "Delta-kappa_gamma",
      "Delta-lambda_gamma"
    },  
    { // Names of coefficients to use 
      "One", 
      "TGCA", "TGCB", "TGCC", // linear
      "TGCD", "TGCE", "TGCF", // quadratic
      "TGCG", "TGCH", "TGCI"  // mixed
    }
  };
  return cTGC_fct_link;
}

//------------------------------------------------------------------------------

PREW::Data::FctLink RKDistrSetup::get_tau_removal_fct_link() const {
  /** Returns instruction that describes how tau removal is built.
  **/
  return { "ConstantCoef", {}, {"TauRemovalFactor"} } ;
}

PREW::Data::FctLink RKDistrSetup::get_nu_and_tau_removal_fct_link() const {
  /** Returns instruction that describes how tau and neutrino removal is built.
  **/
  return { "ConstantCoef", {}, {"NuAndTauRemovalFactor"} };
}

//------------------------------------------------------------------------------

PREW::Data::FctLink RKDistrSetup::get_chi_xs_fct_link(
  const PREW::Data::DistrInfo & info_chi
) const {
  /** Returns instruction that describes how function for free chiral cross 
      section is built.
  **/
  PREW::Data::FctLink chi_xs_fct_link {
    "Constant", // Name of function to use
    { // Names of parameters to use 
      Names::ParNaming::chi_xs_par_name(
        info_chi.m_distr_name,
        info_chi.m_pol_config
      )
    },  
    {} // No coefficients
  };
  return chi_xs_fct_link;
}

//------------------------------------------------------------------------------

PREW::Data::FctLink RKDistrSetup::get_lumi_fraction_fct_link(
  const PREW::Data::DistrInfo & info_pol
) const {
  /** Returns instruction that describes how function for luminosity fraction is
      built.
  **/
  PREW::Data::FctLink lumi_fraction_fct_link {
    "LuminosityFraction", // Name of function to use
    { // Names of parameters to use 
      Names::ParNaming::lumi_name(info_pol.m_energy)
    },  
    { // Names of coefficients to use 
      Names::CoefNaming::lumi_fraction_name(info_pol)
    }
  };
  return lumi_fraction_fct_link;
}

//------------------------------------------------------------------------------

} // Namespace Setups
} // Namespace PrEWUtils