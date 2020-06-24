#include <DataHelp/PredDistrHelp.h>
#include <Names/CoefNaming.h>
#include <Names/ParNaming.h>
#include <Setups/RKDistrSetup.h>

// Includes from PrEW
#include <Data/DistrInfo.h>
#include <Data/DistrUtils.h>
#include <Data/FctLink.h>
#include <GlobalVar/Chiral.h>
#include <Input/InfoRKFile.h>
#include <Input/DataReader.h>

#include "spdlog/spdlog.h"

// Standard library
#include <set>

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

void RKDistrSetup::use_distr(
  const std::string & distr_name,
  const std::string & mode
) {
  /** Select a distribution to be used in the fit and the mode in which it is
      to be used.
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
    Possible modes:
      "differential"
      "summed"
  **/
  m_used_distr_modes[distr_name] = mode;
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
  auto name_cond = [name](const PrEW::Fit::FitPar &par){return par.get_name()==name;};
  if ( 
    find_if(m_common_pars.begin(), m_common_pars.end(), name_cond) 
    != m_common_pars.end() 
  ) {
    spdlog::warn("Lumi for energy {} already set, not resetting", energy);
    return;
  }
  
  this->add_par({name, val, ini_unc}, energy);
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
  PrEW::Fit::FitPar new_pol {name, val, ini_unc};
  this->add_par(new_pol, energy);
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
  (this->find_par(name,energy)).set_constrgauss( constr_val, constr_unc );
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
  (this->find_par(name,energy)).set_constrgauss( constr_val, constr_unc );
}

//------------------------------------------------------------------------------

void RKDistrSetup::fix_lumi(int energy) {
  /** Fix the luminosity parameter at the given energy to its current value.
      Will not be varied by fit.
  **/
  std::string name = Names::ParNaming::lumi_name(energy);
  (this->find_par(name,energy)).fix();
}

//------------------------------------------------------------------------------

void RKDistrSetup::fix_pol(const std::string & name, int energy) {
  /** Fix the polarisation parameter at the given energy to its current value.
      Will not be varied by fit.
  **/
  (this->find_par(name,energy)).fix();
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
    PrEW::Data::PolLink(energy,name,e_pol_name,p_pol_name,e_pol_sign,p_pol_sign)
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
  this->add_par({"Delta-g1Z", 0.0, 0.0001});
  this->add_par({"Delta-kappa_gamma", 0.0, 0.0001});
  this->add_par({"Delta-lambda_gamma", 0.0, 0.0001});
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
  this->add_par(
    {Names::ParNaming::chi_xs_par_name(distr_name,chiral_config), 1.0, 0.0001});
  m_free_xs_chi[distr_name].push_back(chiral_config);
}

//------------------------------------------------------------------------------

void RKDistrSetup::free_total_chiral_xsection(const std::string & distr_name) {
  /** Set the global scaling of all non-zero chiral configurations of this 
      distribution as free. 
      Represents that the total chiral cross section of the corresponding 
      process is a free parameter.
  **/
  PrEW::Fit::FitPar scaling_par {
    Names::ParNaming::total_chi_xs_par_name(distr_name), 1.0, 0.001
  };
  this->add_par(scaling_par);
  m_free_total_xs_chi.push_back(distr_name);
}

//------------------------------------------------------------------------------

void RKDistrSetup::free_asymmetry_2xs( 
  const std::string & distr_name,
  const std::string & chiral_config_0,
  const std::string & chiral_config_1,
  const std::string & asym_name
) {
  /** Set the chiral asymmetry between the two given chiral configurations of 
      the distributions as free parameter.
      Asymmetry definition: A = (xs0 - xs1) / (xs0 + xs1)
      where xs is the sum of all bins of the distribution at the given config.
      Free parameter: DeltaA ( A_meas = A_pred + DeltaA )
      The parameter name can be set if wanted.
  **/
  std::vector<std::string> chiral_configs = {chiral_config_0, chiral_config_1};
  std::vector<std::string> par_names = {};
  if ( asym_name != "default" ) { par_names = {asym_name}; }
  
  m_free_chi_asymms.push_back(
    SetupHelp::ChiAsymmInfo(distr_name, chiral_configs, par_names));
}

void RKDistrSetup::free_asymmetry_3xs( 
  const std::string & distr_name,
  const std::string & chiral_config_0,
  const std::string & chiral_config_1,
  const std::string & chiral_config_2,
  const std::string & asymI_name,
  const std::string & asymII_name
) {
  /** Set the chiral asymmetries between the three given chiral configurations 
      of the distribution as free parameters.
      Asymmetry definition:       
        A_I   = (xs0 + xs1 - xs2) / (xs0 + xs1 + xs2)
        A_II  = (xs0 - xs1 - xs2) / (xs0 + xs1 + xs2)
        where xs is the sum of all bins of the distribution at the given config.
      Free parameters: DeltaA_i ( A_meas = A_pred + DeltaA )
      The parameter name can be set if wanted.
  **/
  std::vector<std::string> chiral_configs = 
    {chiral_config_0, chiral_config_1, chiral_config_2};
    
  // If pars not all default use the given names
  std::vector<std::string> par_names = {};
  if ( (asymI_name != "default") || (asymII_name != "default") ) { 
    par_names = {asymI_name, asymII_name}; 
  }
  
  m_free_chi_asymms.push_back(
    SetupHelp::ChiAsymmInfo(distr_name, chiral_configs, par_names));
}

//------------------------------------------------------------------------------

void 
RKDistrSetup::free_2f_final_state_asymmetry(const std::string &distr_name,
                                            const std::string &asym_name) {
  /** Set the final state asymmetry Af of a 2-fermion distribution as free 
      parameter.
      Af = [ (c_L^f)^2 - (c_R^f)^2 ] / [ (c_L^f)^2 + (c_R^f)^2 ]
  **/
  m_free_Af.push_back(SetupHelp::AfInfo(distr_name, asym_name));
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
    for (const auto & [distr_name, _] : m_used_distr_modes) {    
      this->complete_distr_setup( distr_name, energy );
    } 
  }
  
  this->complete_chi_asymm_setup();
  this->complete_Af_setup();
  
  // For debugging:
  spdlog::debug("RKDistrSetup: Printing results of completed setup");
  spdlog::debug("Using distributions:");
  for ( const auto & used_distr: m_used_distrs ) {
    const auto & info = used_distr.m_info;
    spdlog::debug("{} @ {} & {}", info.m_distr_name, info.m_energy, info.m_pol_config);
    spdlog::debug(" -> First signal value: {}", used_distr.m_sig_distr.at(0));
  }
  spdlog::debug("Using coefficients:");
  for ( const auto & used_coef: m_used_coefs ) {
    const auto & info = used_coef.m_info;
    spdlog::debug("{} for {} @ {} & {}", used_coef.m_coef_name, info.m_distr_name, info.m_energy, info.m_pol_config);
    spdlog::debug(" -> First coef value: {}", used_coef.m_coefficients.at(0));
  }
}

//------------------------------------------------------------------------------

const std::vector<int> & RKDistrSetup::get_energies() const {return m_energies;}

//------------------------------------------------------------------------------

PrEW::Connect::DataConnector RKDistrSetup::get_data_connector() const {
  /** Get the connector that contains all the information that it needs to 
      properly link the predicition functions.
      Must be called _after_ calling final combination.
  **/
  return PrEW::Connect::DataConnector(
    m_used_distrs,
    m_used_coefs,
    m_pred_links,
    m_pol_links
  );
}

//------------------------------------------------------------------------------

PrEW::Fit::ParVec RKDistrSetup::get_pars (int energy) const { 
  /** Get the parameters that are to be used by the prediciton functions at the
      given energy.
      Must be called _after_ calling final combination.
  **/
  PrEW::Fit::ParVec pars = m_common_pars;
  pars.insert(
    pars.end(), 
    m_separate_pars.at(energy).begin(),
    m_separate_pars.at(energy).end()
  );
  return pars; 
}
  
//------------------------------------------------------------------------------

PrEW::Fit::ParVec RKDistrSetup::get_pars () const { 
  /** Get all parameters that are to be used by the prediciton functions at any
      energy.
      Must be called _after_ calling final combination.
  **/
  PrEW::Fit::ParVec pars = m_common_pars;
  for (const auto & [energy, separate_pars]: m_separate_pars) {
    pars.insert( pars.end(), separate_pars.begin(), separate_pars.end() );
  }
  return pars; 
}

//------------------------------------------------------------------------------
// Internal functions
//------------------------------------------------------------------------------

void RKDistrSetup::complete_chi_asymm_setup() {
  /** Complete setting up the chiral asymmetries.
  **/
  
  // Map to collect all involved chiral cross sections, needed to set up coefs
  std::map<std::string, std::set<std::string>> chiral_distrs {};
  
  for ( const auto & chi_asymm: m_free_chi_asymms ) {
    const auto & distr_name = chi_asymm.get_distr_name();
    
    // Collect the needed parameters and function links
    for ( const auto &par: chi_asymm.get_pars() ) { this->add_par(par); }
    
    for ( const auto & chiral_config: chi_asymm.get_chiral_configs() ) {
      // Add the relevant function links
      for ( int energy: m_energies ) {
        auto & pred_link = 
          this->find_pred_link( {distr_name, chiral_config, energy} );
        pred_link.m_fcts_links_sig.push_back(
          chi_asymm.get_fct_link(energy, chiral_config));
      } // Energies loop
      
      // Mark the chiral configs of this distribution so that their total xs is 
      // calculated and setup as coefficient
      chiral_distrs[distr_name].insert(chiral_config);
    } // Chiral config loop
    
  } // Asymmetries loop
  
  // Find and set up all the chiral cross sections sums needed
  for ( int energy: m_energies ) {
    for ( const auto &[distr_name, chiral_configs] : chiral_distrs ) {
      std::vector<std::string> chiral_config_vec (
        chiral_configs.begin(), chiral_configs.end() );
      for (const auto &chiral_config: chiral_configs) {
        PrEW::Data::DistrInfo config_info {distr_name,chiral_config,energy};
        this->add_chi_distr_coefs( config_info, chiral_config_vec, "sum");
      }
    } // Distribution loop
  } // Energy loop
}

//------------------------------------------------------------------------------

void RKDistrSetup::complete_Af_setup() {
  /** Complete setting up the 2-fermion final state asymmetries.
  **/

  // Map to collect all involved chiral cross sections, needed to set up coefs
  std::map<std::string, std::set<std::string>> chiral_distrs {};

  for ( const auto & asymm: m_free_Af ) {
    const auto & distr_name = asymm.get_distr_name();
    const auto & LR_config = asymm.get_LR_config();
    const auto & RL_config = asymm.get_RL_config();

    // Collect the needed parameters and function links
    auto par = asymm.get_par();
    // Only add the parameter if it doesn't already exist
    auto cond = [par](const PrEW::Fit::FitPar &_par){return _par==par;};
    auto par_it = std::find_if(m_common_pars.begin(),m_common_pars.end(),cond);
    if ( par_it == m_common_pars.end() ) { m_common_pars.push_back(par); }

    // Add the relevant function links
    for ( int energy: m_energies ) {
      auto & pred_link_LR = this->find_pred_link({distr_name,LR_config,energy});
      pred_link_LR.m_fcts_links_sig.push_back(
        asymm.get_fct_link(energy, LR_config));

      auto & pred_link_RL = this->find_pred_link({distr_name,RL_config,energy});
      pred_link_RL.m_fcts_links_sig.push_back(
        asymm.get_fct_link(energy, RL_config));
    } // Energies loop

    // Mark the chiral configs of this distribution so it's coefs are collected
    chiral_distrs[distr_name].insert(LR_config);
    chiral_distrs[distr_name].insert(RL_config);
  } // Asymmetries loop

  // Find and set up all the coefficients for all cross sections
  // ADD FUNCTION TO ADD PAR/COEF THAT CHECK IF IT ALREADY EXISTS
  // ADD ALL THREE COEFFICIENTS
  for ( int energy: m_energies ) {
    for ( const auto &[distr_name, chiral_configs] : chiral_distrs ) {
      std::vector<std::string> chiral_config_vec (chiral_configs.begin(), 
                                                  chiral_configs.end());
                                                  
      for (const auto &chiral_config: chiral_configs) {
        PrEW::Data::DistrInfo config_info {distr_name,chiral_config,energy};
        
        this->add_chi_distr_coefs(config_info, chiral_config_vec, "sum");
        this->add_chi_distr_coefs(config_info, {chiral_config}, "differential");
        this->add_costheta_index_coef({distr_name, chiral_config, energy}, 0);
      }
    } // Distribution loop
  } // Energy loop
}

//------------------------------------------------------------------------------

void RKDistrSetup::complete_distr_setup(
  const std::string & distr_name,
  int energy
) {
  /** Complete setting up the instructions, parameters and coefficicents for 
      the given distribution at the given energy.
  **/
  // Mark distribution as used and determine binning of the distributions
  auto distrs = this->determine_distrs(distr_name, energy);
  m_used_distrs.insert( m_used_distrs.end(), distrs.begin(), distrs.end() );
  int n_bins = distrs.at(0).m_bin_centers.size();
  
  // Get coefficients for this distribution and mark them as used
  auto coefs = 
    PrEW::Data::DistrUtils::subvec_energy_and_name( 
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

PrEW::Data::PredDistrVec RKDistrSetup::determine_distrs(
  const std::string & distr_name, int energy
) {
  /** Determine and return the predicted distributions with given name at given 
      energy.
      Depends on the mode in which the distribution is supposed to be used.
  **/
  
  auto distrs = 
    PrEW::Data::DistrUtils::subvec_energy_and_name(
      m_input_distrs, energy, distr_name
    );
    
  // Look in which mode the distribution is requested and change if needed
  std::string mode = m_used_distr_modes.at(distr_name);
  if ( mode == "differential") {
    // Nothing to do, already differential
  } else if ( mode == "summed" ) {
    // Request use of summed up / combined bins (e.g. total xs)
    // -> Transform each distr. into it's total xs only
    PrEW::Data::PredDistrVec rebinned_distrs {};
    for (const auto & distr : distrs) {
      rebinned_distrs.push_back( PrEW::Data::DistrUtils::combine_bins(distr) );
    }
    distrs = rebinned_distrs;
  } else {
    spdlog::warn(
      "Unknown distribution mode {} for distribution {}.\n"
      "-> Will use default differential distribution.", distr_name, mode
    );
  }
  
  return distrs;
}

//------------------------------------------------------------------------------

void RKDistrSetup::complete_chi_setup(
  const PrEW::Data::DistrInfo & info_chi, 
  int n_bins
) {
  /** Set up the links, coefficients and parameters for a given chiral 
      distribution with a given number of bins.
  **/
  // Set up the function links for this chiral configuration
  PrEW::Data::FctLinkVec sig_fct_links {};
  PrEW::Data::FctLinkVec bkg_fct_links {};
  
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
  
  // If chiral cross section is supposed to be free, add cross section parameter
  if ( this->xs_chi_is_free( info_chi ) ) {
    sig_fct_links.push_back( this->get_chi_xs_fct_link( info_chi ) );
  }
  
  // Check if total chiral cross section and chiral asymmetries are supposed to
  // be free -> If so, add the appropriate corresponding scalings
  if ( this->total_chiral_xsection_is_free( info_chi.m_distr_name ) ) {
    sig_fct_links.push_back( this->get_total_chi_xs_fct_link( info_chi ) );
  }

  PrEW::Data::PredLink link_chi { info_chi, sig_fct_links, bkg_fct_links };
  m_pred_links.push_back(link_chi);
}

//------------------------------------------------------------------------------

void RKDistrSetup::complete_pol_setup(
  const PrEW::Data::DistrInfo & info_pol, 
  int n_bins
) {
  /** Set up the links, coefficients and parameters for a given polarised 
      distribution with a given number of bins.
  **/
  // Set up the function links for this polarisation configuration
  PrEW::Data::FctLinkVec sig_fct_links {};
  PrEW::Data::FctLinkVec bkg_fct_links {};
    
  // Add instructions for luminosity factor
  this->add_lumi_fraction_coef(info_pol, n_bins);
  sig_fct_links.push_back( this->get_lumi_fraction_fct_link( info_pol ) );
    
  PrEW::Data::PredLink link_pol { info_pol, sig_fct_links, bkg_fct_links };
  m_pred_links.push_back(link_pol);
}

//------------------------------------------------------------------------------

void RKDistrSetup::read_input_file(const std::string & file_path, int energy) {
  /** Read input from a given file at a given energy.
  **/
  // PrEW interpretable input file info 
  PrEW::Input::InfoRKFile i {file_path, "RK", energy};
  // File reading
  PrEW::Input::DataReader reader(&i);
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

PrEW::Fit::FitPar & RKDistrSetup::find_par_in_vec(
  const std::string & par_name,
  PrEW::Fit::ParVec & vec
) {
  /** Find the fit parameter of given name in the given vector and return a 
      (non-const) reference to it.
  **/
  auto name_cond =
    [par_name](const PrEW::Fit::FitPar &par){return par.get_name()==par_name;};
  auto par_it = std::find_if(vec.begin(), vec.end(), name_cond);
  if ( par_it == vec.end() ) {
    throw std::invalid_argument( ("Couldn't find parameter " + par_name + " in vector!").c_str() );
  }
  return *par_it;
}

PrEW::Fit::FitPar & RKDistrSetup::find_par(const std::string & name) {
  /** Find the parameter of given name in the common parameters and return a 
      (non-const) reference to it.
  **/
  return this->find_par_in_vec(name, m_common_pars);
}

PrEW::Fit::FitPar & RKDistrSetup::find_par(const std::string & name, int energy) {
  /** Find the parameter of given name in the energy-specific parameters and 
      return a (non-const) reference to it.
  **/
  return this->find_par_in_vec(name, m_separate_pars.at(energy));
}

//------------------------------------------------------------------------------

PrEW::Data::PredLink & RKDistrSetup::find_pred_link(
  const PrEW::Data::DistrInfo & info
) {
  /** Find function link to given distribution and return (non-const) reference.  
  **/
  auto info_cond =
    [info](const PrEW::Data::PredLink &link){return link.m_info==info;};
  auto link_it = 
    std::find_if(m_pred_links.begin(), m_pred_links.end(), info_cond);
  
  // If link doesn't exist yet, create it (with empty funciton instructions)
  if ( link_it == m_pred_links.end() ) {
    m_pred_links.push_back( {info, {}, {}} );
  }
  
  return *link_it;
}

//------------------------------------------------------------------------------

PrEW::Data::PredDistr &
RKDistrSetup::find_pred_distr(const PrEW::Data::DistrInfo & info){
  /** Find PredDistr in the used distributions and return (non-const) reference 
      to it.
  **/
  auto info_cond =
    [info](const PrEW::Data::PredDistr &pred){return pred.m_info==info;};
  auto pred_it = 
    std::find_if(m_used_distrs.begin(), m_used_distrs.end(), info_cond);
  
  if ( pred_it == m_used_distrs.end() ) {
    throw std::invalid_argument(("RKDistrSetup: Couldn't find PredDistr: " +
                                info.m_distr_name + " " + info.m_pol_config + 
                                " " + std::to_string(info.m_energy)).c_str());
  }
  
  return *pred_it;
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_par(const PrEW::Fit::FitPar &par) {
  /** Add a parameter to the common parameters of this setup while first 
      checking that it does not already exist.
   **/
  auto cond = [par](const PrEW::Fit::FitPar &_par){return _par==par;};
  auto par_it = 
    std::find_if(m_common_pars.begin(), m_common_pars.end(), cond);
  if ( par_it == m_common_pars.end() ) {
    m_common_pars.push_back(par);
  }
}

void RKDistrSetup::add_par(const PrEW::Fit::FitPar &par, int energy) {
  /** Add a parameter to the separate parameters of this setup while first 
      checking that it does not already exist.
   **/
  auto cond = [par](const PrEW::Fit::FitPar &_par){return _par==par;};
  auto par_it = std::find_if(m_separate_pars[energy].begin(), 
                             m_separate_pars[energy].end(), cond);
  if ( par_it == m_separate_pars[energy].end() ) {
    m_separate_pars[energy].push_back(par);
  }
}

void RKDistrSetup::add_coef(const PrEW::Data::CoefDistr &coef) {
  /** Add a coefficient to this setup while first checking that it does not 
      already exist.
   **/
  auto cond = 
    [coef](const PrEW::Data::CoefDistr &_coef){
      return (_coef.m_coef_name==coef.m_coef_name) && 
             (_coef.m_info==coef.m_info);};
  auto coef_it = 
   std::find_if(m_used_coefs.begin(), m_used_coefs.end(), cond);
  if ( coef_it == m_used_coefs.end() ) {
   m_used_coefs.push_back(coef);
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
    PrEW::Data::DistrUtils::subvec_energy_and_name( 
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
  const PrEW::Data::DistrInfo & info_chi
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

bool RKDistrSetup::total_chiral_xsection_is_free(
  const std::string & distr_name
) const {
  /** Check if the total chiral cross section of the given distribution is 
      supposed to be a free parameter.
  **/
  bool free =
    std::find(
      m_free_total_xs_chi.begin(), 
      m_free_total_xs_chi.end(), 
      distr_name
    ) != m_free_total_xs_chi.end();
  return free;
}

//------------------------------------------------------------------------------

std::vector<PrEW::Data::DistrInfo> RKDistrSetup::get_infos_pol(
  const std::string & distr_name,
  int energy
) const {
  /** Create distribution info containers for the given distribution for all 
      available polarisation configurations at the given energy.
  **/
  std::vector<PrEW::Data::DistrInfo> infos_pol {};
  for ( const auto & pol_link : m_pol_links ) {
    if ( pol_link.get_energy() == energy ) {
      infos_pol.push_back( { distr_name, pol_link.get_pol_config(), energy } );
    }
  }
  return infos_pol;
}

//------------------------------------------------------------------------------

std::vector<PrEW::Data::DistrInfo> RKDistrSetup::get_infos_chi(
  const std::string & distr_name,
  int energy
) const {
  /** Create distribution info containers for the given distribution for all 
      chiral configurations at the given energy.
  **/
  std::vector<PrEW::Data::DistrInfo> infos_chi {
    {distr_name, PrEW::GlobalVar::Chiral::eLpR, energy},
    {distr_name, PrEW::GlobalVar::Chiral::eRpL, energy},
    {distr_name, PrEW::GlobalVar::Chiral::eLpL, energy},
    {distr_name, PrEW::GlobalVar::Chiral::eRpR, energy}
  };
  return infos_chi;
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_chi_distr_coefs(
  const PrEW::Data::DistrInfo & info, 
  const std::vector<std::string> & chiral_configs, const std::string &type) {
  /** Get the summed or differential chiral cross section for the distribution
      with the given info and add a coefficient with it for the other given 
      chiral configurations.
      Type: "sum" or "differential"
   **/
  
  // Find the corresponding distribution
  auto pred = this->find_pred_distr(info);
  int n_bins = pred.m_sig_distr.size();

  // Determine coefficient content
  std::string coef_name = "";
  std::vector<double> coef_vec {};

  if ( type == "sum" ) {
    // Get the chiral signal cross section, which is needed to determine the 
    // asymmetry of the signal
    coef_name = Names::CoefNaming::chi_xs_coef_name( info );
    double xs_chi = DataHelp::PredDistrHelp::get_pred_sum(pred, "signal");
    coef_vec = std::vector<double>(n_bins,xs_chi);
  } else if ( type == "differential" ) {
    // Differential coefficient only created for its own distribution
    coef_name = Names::CoefNaming::chi_distr_coef_name(info, "signal"),
    coef_vec = DataHelp::PredDistrHelp::pred_to_coef(pred, "signal");
  } else {
    throw std::invalid_argument(
      ("RKDistrSetup: Invalid distr coef type requested: " + type).c_str());
  }
  
  // Create coefficient for all distributions that need it
  for ( const auto & other_config: chiral_configs ) {
    PrEW::Data::DistrInfo new_info {info.m_distr_name, other_config, 
                                    info.m_energy};
    PrEW::Data::CoefDistr new_coef { coef_name, new_info, coef_vec };
    this->add_coef(new_coef);
  }
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_costheta_index_coef(const PrEW::Data::DistrInfo & info,
                                           int costheta_index) {
  /** Add a coefficient describing which index in the observables vector for 
      this distribution describes the cos(Theta) coordinate.
   **/                                            
   int n_bins = this->find_pred_distr(info).m_bin_centers.size();
   this->add_coef({Names::CoefNaming::costheta_index_coef_name(), info, 
                   std::vector<double>(n_bins,costheta_index)});
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_unity_coef( 
  const PrEW::Data::DistrInfo & info,
  int n_bins
) {
  /** Add a coefficient which is simply 1.0 for each bin of the distribution 
      described by the given info.
  **/
  // TODO ADD CONVENTION FOR "ONE" PARAMETER TO NAMES NAMESPACE
  this->add_coef({"One", info, std::vector<double>(n_bins,1.0)});
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_tau_removal_coef(
  const PrEW::Data::DistrInfo & info,
  int n_bins
) {
  /** Add the coefficient that is needed to remove taus from the muon/tau 
      mixture of W decays.
  **/
  // TODO COEFFICIENT CONVENTIONS IN NAMES NAMESPACE!!!
  double tau_removal = 0.5; // Remove tau from tau/muon mixture
  this->add_coef(
    {"TauRemovalFactor", info, std::vector<double>(n_bins,tau_removal)});
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_nu_and_tau_removal_coef(
  const PrEW::Data::DistrInfo & info,
  int n_bins
) {
  /** Add the coefficient that is needed to remove neutrinos (2. & 3. gen) and 
      taus from mixture of 2. & 3. gen. lepton decays of Z bosons.
  **/
  // TODO COEFFICIENT CONVENTIONS IN NAMES NAMESPACE!!!
  double nu_and_tau_removal = 0.168;
  this->add_coef({"NuAndTauRemovalFactor", info, 
                  std::vector<double>(n_bins,nu_and_tau_removal)});
}

//------------------------------------------------------------------------------

void RKDistrSetup::add_lumi_fraction_coef(
  const PrEW::Data::DistrInfo & info_pol,
  int n_bins
) {
  /** Add the coefficient for the luminosity fraction of a given distribution 
      (for each bin).
  **/
  this->add_coef(
    { Names::CoefNaming::lumi_fraction_name(info_pol), info_pol,
      std::vector<double>( 
        n_bins, 
        m_lumi_fractions.at(info_pol.m_energy).at(info_pol.m_pol_config) 
      ) });
}

//------------------------------------------------------------------------------

// TODO THIS IN ITS OWN NAMESPACE
PrEW::Data::FctLink RKDistrSetup::get_cTGC_fct_link() const {
  /** Returns instruction that describes how cTGC parameterisation is built.
  **/
  PrEW::Data::FctLink cTGC_fct_link {
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

PrEW::Data::FctLink RKDistrSetup::get_tau_removal_fct_link() const {
  /** Returns instruction that describes how tau removal is built.
  **/
  return { "ConstantCoef", {}, {"TauRemovalFactor"} } ;
}

PrEW::Data::FctLink RKDistrSetup::get_nu_and_tau_removal_fct_link() const {
  /** Returns instruction that describes how tau and neutrino removal is built.
  **/
  return { "ConstantCoef", {}, {"NuAndTauRemovalFactor"} };
}

//------------------------------------------------------------------------------

PrEW::Data::FctLink RKDistrSetup::get_chi_xs_fct_link(
  const PrEW::Data::DistrInfo & info_chi
) const {
  /** Returns instruction that describes how function for free chiral cross 
      section is built.
  **/
  PrEW::Data::FctLink chi_xs_fct_link {
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

PrEW::Data::FctLink RKDistrSetup::get_total_chi_xs_fct_link( 
  const PrEW::Data::DistrInfo & info_chi
) const {
  /** Returns instruction that describes how function for free total chiral 
      cross section is built.
  **/
  PrEW::Data::FctLink total_chi_xs_fct_link {
    "Constant", // Name of function to use
    { // Names of parameters to use 
      Names::ParNaming::total_chi_xs_par_name( info_chi.m_distr_name )
    },  
    {} // No coefficients
  };
  return total_chi_xs_fct_link;
}

//------------------------------------------------------------------------------

PrEW::Data::FctLink RKDistrSetup::get_lumi_fraction_fct_link(
  const PrEW::Data::DistrInfo & info_pol
) const {
  /** Returns instruction that describes how function for luminosity fraction is
      built.
  **/
  PrEW::Data::FctLink lumi_fraction_fct_link {
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