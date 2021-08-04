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
      ef_name{def_name}, k0_name{def_name}, dk_name{def_name};

  // Starting values of 2f parameters (for given distribution)
  static constexpr double s0_def{1.0}, Ae_def{0}, Af_def{0}, ef_def{0},
      k0_def{0}, dk_def{0};
  double s0_val{s0_def}, Ae_val{Ae_def}, Af_val{Af_def}, ef_val{ef_def},
      k0_val{k0_def}, dk_val{dk_def};

  // Index of cosine theta coordinate
  int _costheta_index{0};

  // Potential parameter constraints
  using ParConstr = std::pair<double, double>;
  ParConstr s0_constr{}, Ae_constr{}, Af_constr{}, ef_constr{}, k0_constr{},
      dk_constr{};

  // Potential parameter fixing
  bool s0_fixed{}, Ae_fixed{}, Af_fixed{}, ef_fixed{}, k0_fixed{}, dk_fixed{};

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
  DifermionPars &k0(const std::string &name, double val = k0_def) {
    k0_name = name; k0_val = val; return *this; }
  DifermionPars &dk(const std::string &name, double val = dk_def) {
    dk_name = name; dk_val = val; return *this; }

  DifermionPars &s0(double val) { s0_val = val; return *this; }
  DifermionPars &Ae(double val) { Ae_val = val; return *this; }
  DifermionPars &Af(double val) { Af_val = val; return *this; }
  DifermionPars &ef(double val) { ef_val = val; return *this; }
  DifermionPars &k0(double val) { k0_val = val; return *this; }
  DifermionPars &dk(double val) { dk_val = val; return *this; }

  DifermionPars &costheta_index(int val) {_costheta_index = val; return *this;}
  
  DifermionPars &constr_s0(double val, double unc) {
    s0_constr = {val,unc}; return *this; }
  DifermionPars &constr_Ae(double val, double unc) {
    Ae_constr = {val,unc}; return *this; }
  DifermionPars &constr_Af(double val, double unc) {
    Af_constr = {val,unc}; return *this; }
  DifermionPars &constr_ef(double val, double unc) {
    ef_constr = {val,unc}; return *this; }
  DifermionPars &constr_k0(double val, double unc) {
    k0_constr = {val,unc}; return *this; }
  DifermionPars &constr_dk(double val, double unc) {
    dk_constr = {val,unc}; return *this; }
  
  DifermionPars &fix_s0() { s0_fixed = true; return *this; }
  DifermionPars &fix_Ae() { Ae_fixed = true; return *this; }
  DifermionPars &fix_Af() { Af_fixed = true; return *this; }
  DifermionPars &fix_ef() { ef_fixed = true; return *this; }
  DifermionPars &fix_k0() { k0_fixed = true; return *this; }
  DifermionPars &fix_dk() { dk_fixed = true; return *this; }
  
  DifermionPars &fix_s0(double val) { s0_fixed = true; return this->s0(val); }
  DifermionPars &fix_Ae(double val) { Ae_fixed = true; return this->Ae(val); }
  DifermionPars &fix_Af(double val) { Af_fixed = true; return this->Af(val); }
  DifermionPars &fix_ef(double val) { ef_fixed = true; return this->ef(val); }
  DifermionPars &fix_k0(double val) { k0_fixed = true; return this->k0(val); }
  DifermionPars &fix_dk(double val) { dk_fixed = true; return this->dk(val); }
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