#ifndef LIB_RKDISTRSETUP_H
#define LIB_RKDISTRSETUP_H 1

// Includes from PrEW
#include "Connect/DataConnector.h"
#include "Data/CoefDistr.h"
#include "Data/PolLink.h"
#include "Data/PredDistr.h"
#include "Data/PredLink.h"
#include "Fit/FitPar.h"

#include <string>
#include <map>

namespace PrEWUtils {
namespace Setups {
  
  class RKDistrSetup {
    /** Class to read the files provided by Robert Karl and use the output in 
        PrEW.
        Supposed to work as usable interface for understandle execution of fits.
    **/
    std::vector<std::string> m_input_files {};
    std::vector<int> m_energies {};
    
    // Data from RK files
    PrEW::Data::PredDistrVec  m_input_distrs {};
    PrEW::Data::CoefDistrVec  m_input_coefs {};
    
    // Data to be used in fit
    PrEW::Data::PredDistrVec  m_used_distrs {};
    PrEW::Data::CoefDistrVec  m_used_coefs {};
    
    PrEW::Data::PolLinkVec    m_pol_links {};
    PrEW::Data::PredLinkVec   m_pred_links {};
    
    PrEW::Fit::ParVec         m_common_pars {};
    std::map<int, PrEW::Fit::ParVec> m_separate_pars {};
    
    // Internal trackers
    std::vector<std::string> m_used_distr_names {};
    std::map<int, std::map<std::string,double>> m_lumi_fractions {};
    
    bool m_use_cTGCs {};
    bool m_WW_mu_only {};
    bool m_ZZ_mu_only {};
    
    // Map tracking which chiral cross sections should be free for each distr
    std::map<std::string,std::vector<std::string>> m_free_xs_chi {};
    
    // Tracking which total chiral cross sections and asymmetries are free
    std::vector<std::string> m_free_total_xs_chi {};
    std::map<std::string,std::vector<std::string>> m_free_asymmetries {};
    
    public: 
      // Constructor
      RKDistrSetup();
      RKDistrSetup(const std::string & file_path, int energy);
      
      // Add input
      void add_input_file(const std::string & file_path);
      void add_energy(int energy);
    
      // Functions determining how setup looks
      void use_distr(const std::string & distr_name);
    
      void set_lumi(int energy, double val, double ini_unc);
      
      void add_pol(
        const std::string & name,
        int energy,
        double val,
        double ini_unc
      );
      void add_pol_config(
        const std::string & name,
        int energy,
        const std::string & e_pol_name,
        const std::string & p_pol_name,
        const std::string & e_pol_sign,
        const std::string & p_pol_sign,
        double lumi_fraction
      );
      
      void add_lumi_constr(int energy, double constr_val, double constr_unc);
      void add_pol_constr(
        const std::string & name,
        int energy,
        double constr_val,
        double constr_unc
      );
      
      void fix_lumi(int energy);
      void fix_pol(const std::string & name, int energy);
      
      void activate_cTGCs();
      
      void free_chiral_xsection( 
        const std::string & distr_name,
        const std::string & chiral_config 
      );
      
      void free_total_chiral_xsection( const std::string & distr_name );
      void free_asymmetry( 
        const std::string & distr_name,
        const std::string & chiral_config_0,
        const std::string & chiral_config_1
      );
      void free_asymmetry( 
        const std::string & distr_name,
        const std::string & chiral_config_0,
        const std::string & chiral_config_1,
        const std::string & chiral_config_2
      );
      void free_asymmetry( 
        const std::string & distr_name,
        const std::string & chiral_config_0,
        const std::string & chiral_config_1,
        const std::string & chiral_config_2,
        const std::string & chiral_config_3
      );
      
      void set_WW_mu_only();
      void set_ZZ_mu_only();
      

      // Finishing the setup
      void complete_setup();
      
      // Get result
      const std::vector<int> & get_energies() const;
      PrEW::Connect::DataConnector get_data_connector() const;
      
      PrEW::Fit::ParVec get_pars (int energy) const;
      PrEW::Fit::ParVec get_pars () const;
      
    protected:
      // Internal functions
      
      // Input-related
      void read_input_file(const std::string & file_path, int energy);
      void read_input_files();
      
      // Internal helpers
      PrEW::Fit::FitPar & find_par_in_vec(
        const std::string & par_name,
        PrEW::Fit::ParVec & vec
      );
      PrEW::Fit::FitPar & find_par(const std::string & name);
      PrEW::Fit::FitPar & find_par(const std::string & name, int energy);
      
      // Linking related
      void complete_distr_setup(const std::string & distr_name, int energy);
      void complete_chi_setup(
        const PrEW::Data::DistrInfo & info_chi, 
        int n_bins
      );
      void complete_pol_setup(
        const PrEW::Data::DistrInfo & info_pol, 
        int n_bins
      );
      
      bool has_cTGCs_available(
        const std::string & distr_name, 
        int energy
      ) const;
      bool xs_chi_is_free(
        const PrEW::Data::DistrInfo & info_chi
      ) const;
      bool total_chiral_xsection_is_free(const std::string & distr_name) const;
      bool asymm_is_free( const PrEW::Data::DistrInfo & info_chi ) const;
      
      std::vector<PrEW::Data::DistrInfo> get_infos_pol(
        const std::string & distr_name,
        int energy
      ) const;
      std::vector<PrEW::Data::DistrInfo> get_infos_chi(
        const std::string & distr_name,
        int energy
      ) const;
      
      void add_asymm_par( const std::string & par_name );
      
      void add_chi_xs_sum_coef(
        const PrEW::Data::DistrInfo & info_chi,
        int n_bins
      );
      void add_unity_coef(const PrEW::Data::DistrInfo & info, int n_bins);
      void add_tau_removal_coef(const PrEW::Data::DistrInfo & info, int n_bins);
      void add_nu_and_tau_removal_coef(
        const PrEW::Data::DistrInfo & info,
        int n_bins
      );
      void add_lumi_fraction_coef(
        const PrEW::Data::DistrInfo & info_pol,
        int n_bins
      );
      
      PrEW::Data::FctLink get_cTGC_fct_link() const;
      PrEW::Data::FctLink get_tau_removal_fct_link() const;
      PrEW::Data::FctLink get_nu_and_tau_removal_fct_link() const;
      PrEW::Data::FctLink get_chi_xs_fct_link( 
        const PrEW::Data::DistrInfo & info_chi
      ) const;
      PrEW::Data::FctLink get_total_chi_xs_fct_link( 
        const PrEW::Data::DistrInfo & info_chi
      ) const;
      PrEW::Data::FctLink get_asymm_fct_link( 
        const PrEW::Data::DistrInfo & info_chi
      ) const;
      PrEW::Data::FctLink get_lumi_fraction_fct_link(
        const PrEW::Data::DistrInfo & info_pol
      ) const;
  };
  
} // Namespace Setups
} // Namespace PrEWUtils

#endif