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
  inline static const std::string def_name = "default";
  std::string s0_name{def_name}, Ae_name{def_name}, Af_name{def_name},
      ef_name{def_name}, kL_name{def_name}, kR_name{def_name};

  // Starting values of 2f parameters (for given distribution)
  static constexpr double s0_def{1.0}, Ae_def{0}, Af_def{0}, ef_def{0},
      kL_def{0}, kR_def{0};
  double s0_val{s0_def}, Ae_val{Ae_def}, Af_val{Af_def}, ef_val{ef_def},
      kL_val{kL_def}, kR_val{kR_def};

  // Index of cosine theta coordinate
  int _costheta_index{0};

  // clang-format off
  // Optional arguments
  DifermionPars &s0(const std::string &name, double val = s0_def) {
    s0_name = name; s0_val = val; return *this; }
  DifermionPars &Ae(const std::string &name, double val = Ae_def) {
    Ae_name = name; Ae_val = val; return *this; }
  DifermionPars &Af(const std::string &name, double val = Af_def) {
    Af_name = name; Af_val = val; return *this; }
  DifermionPars &ef(const std::string &name, double val = ef_def) {
    ef_name = name; ef_val = val; return *this; }
  DifermionPars &kL(const std::string &name, double val = kL_def) {
    kL_name = name; kL_val = val; return *this; }
  DifermionPars &kR(const std::string &name, double val = kR_def) {
    kR_name = name; kR_val = val; return *this; }

  DifermionPars &s0(double val) { s0_val = val; return *this; }
  DifermionPars &Ae(double val) { Ae_val = val; return *this; }
  DifermionPars &Af(double val) { Af_val = val; return *this; }
  DifermionPars &ef(double val) { ef_val = val; return *this; }
  DifermionPars &kL(double val) { kL_val = val; return *this; }
  DifermionPars &kR(double val) { kR_val = val; return *this; }

  DifermionPars &costheta_index(int val) {_costheta_index = val; return *this;}
  
  // clang-format on
};

class DifermionParamInfo {
  /** Class describing the information for difermion parametrisation using the
      generalised differential cross section formula.
   **/
  std::string m_distr_name{};
  DifermionPars m_par_info{};

  PrEW::Fit::ParVec m_pars{};
  PrEW::Data::PredLinkVec m_pred_links{};

public:
  // Constructors
  DifermionParamInfo(const std::string &distr_name,
                     const DifermionPars &par_info = DifermionPars());

  // Access functions
  const PrEW::Fit::ParVec &get_pars() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec
  get_coefs(const PrEW::Data::PredDistrVec &preds) const;

protected:
  PrEW::Data::FctLink get_fct_link(const PrEW::Data::DistrInfo &info) const;

  PrEW::Data::DistrInfo get_LR_info(int energy) const;
  PrEW::Data::DistrInfo get_RL_info(int energy) const;
};

using DifermionParamInfoVec = std::vector<DifermionParamInfo>;

} // namespace SetupHelp
} // namespace PrEWUtils

#endif