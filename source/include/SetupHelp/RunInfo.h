#ifndef LIB_RUNINFO_H
#define LIB_RUNINFO_H 1

// includes from PrEW
#include <Data/CoefDistr.h>
#include <Data/DistrInfo.h>
#include <Data/PolLink.h>
#include <Data/PredLink.h>
#include <Fit/FitPar.h>

// Standard library
#include <string>

namespace PrEWUtils {
namespace SetupHelp {

class RunInfo {
  /** Information about the collider run (lumi & polarisation).
   **/
  int m_energy{};
  PrEW::Fit::ParVec m_pars{};
  PrEW::Data::CoefDistrVec m_coefs{};
  PrEW::Data::PolLinkVec m_pol_links{};
  PrEW::Data::PredLinkVec m_pred_links{};

  // Placeholders in coefs and links
  std::string m_default_name{"default"};

public:
  // Constructor
  RunInfo(){};
  RunInfo(int energy);

  // Adding info
  void set_lumi(double val, double ini_unc = 1);
  void add_pol(const std::string &name, double val, double ini_unc = 0.0001);
  void add_pol_config(const std::string &config_name,
                      const std::string &e_pol_name,
                      const std::string &p_pol_name,
                      const std::string &e_pol_sign,
                      const std::string &p_pol_sign, double lumi_fraction);

  void add_lumi_constr(double constr_val, double constr_unc);
  void add_pol_constr(const std::string &name, double constr_val,
                      double constr_unc);

  void fix_lumi();
  void fix_pol(const std::string &name);

  // Access functions
  const PrEW::Fit::ParVec &get_pars() const;
  const PrEW::Data::PolLinkVec &get_pol_links() const;

  PrEW::Data::PredLinkVec
  get_pred_links(const PrEW::Data::InfoVec &infos) const;
  PrEW::Data::CoefDistrVec
  get_coefs(const PrEW::Data::InfoVec &infos) const;

  // Operators
  RunInfo &operator=(const RunInfo &) = default;

protected:
  PrEW::Fit::FitPar &find_par(const std::string &name);
};

} // namespace SetupHelp
} // namespace PrEWUtils

#endif
