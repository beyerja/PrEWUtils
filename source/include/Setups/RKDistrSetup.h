#ifndef LIB_RKDISTRSETUP_H
#define LIB_RKDISTRSETUP_H 1

#include <SetupHelp/AfInfo.h>
#include <SetupHelp/ChiAsymmInfo.h>
#include <SetupHelp/TGCInfo.h>

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
    std::map<std::string, std::string> m_used_distr_modes {};
    std::map<int, std::map<std::string,double>> m_lumi_fractions {};
    
    bool m_WW_mu_only {};
    bool m_ZZ_mu_only {};
    
    // Internal tracking for which parameters should be free
    std::map<std::string,std::vector<std::string>> m_free_xs_chi {};
    std::vector<std::string> m_free_total_xs_chi {};
    std::vector<SetupHelp::ChiAsymmInfo> m_free_chi_asymms {};
    std::vector<SetupHelp::AfInfo> m_free_Af {};
    
    bool m_use_cTGCs {};
    SetupHelp::TGCInfo m_TGC_info;
    
    public: 
      // Constructor
      RKDistrSetup();
      RKDistrSetup(const std::string & file_path, int energy);
      
      // Add input
      void add_input_file(const std::string & file_path);
      void add_energy(int energy);
    
      // Functions determining how setup looks
      void use_distr(
        const std::string & distr_name, 
        const std::string & mode="differential" // TODO TODO TODO IMPLEMENT INCLUDING OPTION FOR COMBINED XS
      );
    
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
      
      void activate_cTGCs(const std::string & mode = "linear");
      
      void free_chiral_xsection( 
        const std::string & distr_name,
        const std::string & chiral_config 
      );
      
      void free_total_chiral_xsection( const std::string & distr_name );
      void free_asymmetry_2xs( 
        const std::string & distr_name,
        const std::string & chiral_config_0,
        const std::string & chiral_config_1,
        const std::string & asym_name = "default"
      );
      void free_asymmetry_3xs( 
        const std::string & distr_name,
        const std::string & chiral_config_0,
        const std::string & chiral_config_1,
        const std::string & chiral_config_2,
        const std::string & asymI_name = "default",
        const std::string & asymII_name = "default"
      );
      void
      free_2f_final_state_asymmetry(const std::string &distr_name,
                                    const std::string &asym_name = "default");

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
      PrEW::Data::PredLink & find_pred_link(const PrEW::Data::DistrInfo & info);
      PrEW::Data::PredDistr &find_pred_distr(const PrEW::Data::DistrInfo & info);
      
      void add_par(const PrEW::Fit::FitPar &par);
      void add_par(const PrEW::Fit::FitPar &par, int energy);
      void add_pars(const PrEW::Fit::ParVec &pars);
      void add_pars(const PrEW::Fit::ParVec &pars, int energy);
      void add_coef(const PrEW::Data::CoefDistr &coef);
      
      // Linking related
      void complete_distr_setup(const std::string & distr_name, int energy);
      void complete_chi_asymm_setup();
      void complete_Af_setup();
      
      PrEW::Data::PredDistrVec determine_distrs(
        const std::string & distr_name, int energy
      );
      
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
      
      std::vector<PrEW::Data::DistrInfo> get_infos_pol(
        const std::string & distr_name,
        int energy
      ) const;
      std::vector<PrEW::Data::DistrInfo> get_infos_chi(
        const std::string & distr_name,
        int energy
      ) const;
      
      void add_chi_distr_coefs(const PrEW::Data::DistrInfo & info, 
                               const std::vector<std::string> & chiral_configs, 
                               const std::string &type);
      void add_costheta_index_coef(const PrEW::Data::DistrInfo & info, 
                                   int costheta_index);
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
      
      PrEW::Data::FctLink get_tau_removal_fct_link() const;
      PrEW::Data::FctLink get_nu_and_tau_removal_fct_link() const;
      PrEW::Data::FctLink get_chi_xs_fct_link( 
        const PrEW::Data::DistrInfo & info_chi
      ) const;
      PrEW::Data::FctLink get_total_chi_xs_fct_link( 
        const PrEW::Data::DistrInfo & info_chi
      ) const;
      PrEW::Data::FctLink get_lumi_fraction_fct_link(
        const PrEW::Data::DistrInfo & info_pol
      ) const;
  };
  
} // Namespace Setups
} // Namespace PrEWUtils

#endif