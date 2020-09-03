#include <SetupHelp/TGCInfo.h>

#include <string>

#include "spdlog/spdlog.h"

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------
// Constructors

TGCInfo::TGCInfo(const std::vector<std::string> &distrs,
                 const std::string &mode)
    : m_mode(mode), m_distrs(distrs) {
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

  if (m_mode == "linear") {
    fct_name = "Linear3DPolynomial_Coeff";
    coef_names = {"One", "TGCA", "TGCB", "TGCC"};
  } else if (m_mode == "quadratic") {
    fct_name = "Quadratic3DPolynomial_Coeff";
    coef_names = {
        "One",  "TGCA", "TGCB", "TGCC", // linear
        "TGCD", "TGCE", "TGCF",         // quadratic
        "TGCG", "TGCH", "TGCI"          // mixed
    };
  } else {
    throw std::invalid_argument("TGCInfo: Unknown mode " + m_mode);
  }

  PrEW::Data::FctLink fct_link{fct_name, par_names, coef_names};
  // Add the generic prediction link (info set later)
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