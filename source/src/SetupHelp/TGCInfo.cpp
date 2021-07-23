#include <SetupHelp/TGCInfo.h>

#include <map>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------

struct CoefNaming {
  /** Small struct holding the names of the linear and quadratic coefs for a
      given style (~ convention).
   **/
  std::vector<std::string> linear{};
  std::vector<std::string> quadratic{};
};

const CoefNaming RK_coef_names{
    // Coefficient Names used by Robert Karl
    {"TGCA", "TGCB", "TGCC"}, // linear
    {"TGCD", "TGCE", "TGCF",  // quadratic
     "TGCG", "TGCH", "TGCI"}  // mixed
};
const CoefNaming JB_coef_names{
    // Coefficient Names used by Jakob Beyer
    {"TGC_k_g", "TGC_k_k", "TGC_k_l"},   // linear
    {"TGC_k_g2", "TGC_k_k2", "TGC_k_l2", // quadratic
     "TGC_k_gk", "TGC_k_gl", "TGC_k_kl"} // mixed
};

//------------------------------------------------------------------------------
// Constructors

TGCInfo::TGCInfo(const std::vector<std::string> &distrs,
                 const std::string &mode, const std::string &style)
    : m_distrs(distrs) {
  /** Constructor needs a vector with the affected distributions,
      as well as:
      > mode ... parametrisation mode
        -> Options: 
            - linear 
            - quadratic
      > style ... style of naming convention
        -> Options:
            - RK : style that Robert Karl used
            - JB : style that Jakob Beyer used
   **/

  // Create the parameters
  std::vector<std::string> par_names = {"Delta-g1Z", "Delta-kappa_gamma",
                                        "Delta-lambda_gamma"};

  for (const auto &par_name : par_names) {
    m_pars.push_back(PrEW::Fit::FitPar(par_name, 0, 0.0001));
  }

  // Add the needed additional coefficients (info set later)
  m_coefs.push_back(PrEW::Data::CoefDistr("One", {}, 1.0));

  // Create the prediction link
  std::string fct_name{};
  std::vector<std::string> coef_names{};

  // Figure out which TGC coefficient naming convention to use
  CoefNaming coef_naming{};
  if (style == "RK") {
    coef_naming = RK_coef_names;
  } else if (style == "JB") {
    coef_naming = JB_coef_names;
  } else {
    throw std::invalid_argument("TGCInfo: Unknown style " + mode);
  }

  // Figure out the correct function name and collect the need coef names
  coef_names = {"One"};
  if (mode == "linear") {
    fct_name = "Linear3DPolynomial_Coeff";
    coef_names.insert(coef_names.end(), coef_naming.linear.begin(),
                      coef_naming.linear.end());
  } else if (mode == "quadratic") {
    fct_name = "Quadratic3DPolynomial_Coeff";
    coef_names.insert(coef_names.end(), coef_naming.linear.begin(),
                      coef_naming.linear.end());
    coef_names.insert(coef_names.end(), coef_naming.quadratic.begin(),
                      coef_naming.quadratic.end());
  } else {
    throw std::invalid_argument("TGCInfo: Unknown mode " + mode);
  }

  // Add the prediction link (info set later)
  PrEW::Data::FctLink fct_link{fct_name, par_names, coef_names};
  m_pred_links.push_back(PrEW::Data::PredLink{{}, {fct_link}, {}});
}

//------------------------------------------------------------------------------
// Functions to use asymmetry in PrEW

const PrEW::Fit::ParVec &TGCInfo::get_pars() const { return m_pars; }

PrEW::Data::PredLinkVec
TGCInfo::get_pred_links(const PrEW::Data::InfoVec &infos) const {
  /** Return all predicition links relevant to the given distributions.
   **/
  PrEW::Data::PredLinkVec pred_links{};

  // Find which distributions are affected and add the links for them
  for (const auto &info : infos) {
    if (this->affects_distr(info)) {
      for (auto pred_link : m_pred_links) {
        pred_link.m_info = info;
        pred_links.push_back(pred_link);
      }
    }
  }

  return pred_links;
}

PrEW::Data::CoefDistrVec
TGCInfo::get_coefs(const PrEW::Data::InfoVec &infos) const {
  /** Return coefficients for all given distributions.
   **/
  PrEW::Data::CoefDistrVec coefs{};

  // Find which distributions are affected and add the coefs for them
  for (const auto &info : infos) {
    if (this->affects_distr(info)) {
      for (auto coef : m_coefs) {
        coef.set_info(info);
        coefs.push_back(coef);
      }
    }
  }

  return coefs;
}

//------------------------------------------------------------------------------

bool TGCInfo::affects_distr(const PrEW::Data::DistrInfo &info) const {
  /** Check if the given distribution is affected by these TGCs.
   **/
  auto name_it = std::find(m_distrs.begin(), m_distrs.end(), info.m_distr_name);
  return name_it != m_distrs.end();
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils