#ifndef LIB_DIFERMIONPARAMINFO_H
#define LIB_DIFERMIONPARAMINFO_H 1

// Includes from PrEW
#include <Data/CoefDistr.h>
#include <Data/FctLink.h>
#include <Data/PredDistr.h>
#include <Data/PredLink.h>
#include <Fit/FitPar.h>
#include <GlobalVar/Chiral.h>

// Standard library
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

struct DifermionPars {
  /** Helper struct to define the (potentially common) parameters of the 
      2f distributions.
      Essentially allows kinda Python-style optional arguments.
      Inspired by:
        https://softwareengineering.stackexchange.com/questions/329709/python-style-keyword-args-in-c-good-practice-or-bad-idea
   **/
  // Names of 2f parameters (for given distribution)
  std::string s0_name {"default"};
  std::string Ae_name {"default"};
  std::string Af_name {"default"};
  std::string ef_name {"default"};
  std::string kL_name {"default"};
  std::string kR_name {"default"};
  
  // Starting values of 2f parameters (for given distribution)
  static constexpr double s0_def = 10000.;
  static constexpr double Ae_def = 0;
  static constexpr double Af_def = 0;
  static constexpr double ef_def = 0;
  static constexpr double kL_def = 0;
  static constexpr double kR_def = 0;
  double s0_val {s0_def};
  double Ae_val {Ae_def};
  double Af_val {Af_def};
  double ef_val {ef_def};
  double kL_val {kL_def};
  double kR_val {kR_def};
  
  // Index of cosine theta coordinate
  int _costheta_index {0};

  // Optional arguments
  DifermionPars& s0(const std::string &name, double val=s0_def) { s0_name = name; s0_val=val; return *this; }
  DifermionPars& Ae(const std::string &name, double val=Ae_def) { Ae_name = name; Ae_val=val; return *this; }
  DifermionPars& Af(const std::string &name, double val=Af_def) { Af_name = name; Af_val=val; return *this; }
  DifermionPars& ef(const std::string &name, double val=ef_def) { ef_name = name; ef_val=val; return *this; }
  DifermionPars& kL(const std::string &name, double val=kL_def) { kL_name = name; kL_val=val; return *this; }
  DifermionPars& kR(const std::string &name, double val=kR_def) { kR_name = name; kR_val=val; return *this; }
  
  DifermionPars& s0(double val) { s0_val=val; return *this; }
  DifermionPars& Ae(double val) { Ae_val=val; return *this; }
  DifermionPars& Af(double val) { Af_val=val; return *this; }
  DifermionPars& ef(double val) { ef_val=val; return *this; }
  DifermionPars& kL(double val) { kL_val=val; return *this; }
  DifermionPars& kR(double val) { kR_val=val; return *this; }
  
  DifermionPars& costheta_index(int val) { _costheta_index=val; return *this; }
};

class DifermionParamInfo {
  std::string m_distr_name{};
  DifermionPars m_par_info{};
  
  PrEW::Fit::ParVec m_pars {};
  PrEW::Data::PredLinkVec m_pred_links{};

public:
  // Constructors
  DifermionParamInfo(const std::string &distr_name,
                     const DifermionPars &par_info = DifermionPars());

  // Access functions
  const PrEW::Fit::ParVec & get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec
  get_coefs(const PrEW::Data::PredDistrVec &preds) const;

protected:
  PrEW::Data::FctLink get_fct_link(const PrEW::Data::DistrInfo &info) const;
};

using DifermionParamInfoVec = std::vector<DifermionParamInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif