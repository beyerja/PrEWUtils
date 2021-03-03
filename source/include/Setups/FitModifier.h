#ifndef LIB_FITMODIFIER_H
#define LIB_FITMODIFIER_H 1

#include <SetupHelp/AfInfo.h>
#include <SetupHelp/DifermionParamInfo.h>
#include <SetupHelp/ParOrder.h>

// includes from PrEW
#include <Connect/DataConnector.h>
#include <Data/CoefDistr.h>
#include <Data/PolLink.h>
#include <Data/PredDistr.h>
#include <Data/PredLink.h>
#include <Fit/FitPar.h>

namespace PrEWUtils {
namespace Setups {

class FitModifier {
  /** Class that modifies a given a fit setup after it was fully assembled.
      Necessary e.g. for modifying the fit setup wrt. the toy generator.
  **/

  int m_energy{};

  // Optional modification specifiers
  SetupHelp::AfInfoVec m_Af_infos{};
  SetupHelp::DifermionParamInfoVec m_difermion_param_infos{};

  // Output specifiers
  SetupHelp::ParOrder::Ordering m_par_ordering{
      SetupHelp::ParOrder::default_ordering};
  SetupHelp::ParOrder::IDMap m_par_id_map{SetupHelp::ParOrder::default_par_map};

public:
  // Constructors
  FitModifier(int energy);

  // Access functions
  int get_energy() const;

  // Instructions on what to modify
  void add(SetupHelp::AfInfo info);
  void add(SetupHelp::DifermionParamInfo info);

  void set_par_ordering(const SetupHelp::ParOrder::Ordering &ordering,
                        const SetupHelp::ParOrder::IDMap &id_map =
                            SetupHelp::ParOrder::default_par_map);

  // Use the given instruction to change a setup
  void modify_setup(PrEW::Connect::DataConnector *connector,
                    PrEW::Fit::ParVec *pars) const;

protected:
  // Internal functions
  void apply_Af_mod(PrEW::Connect::DataConnector *connector,
                    PrEW::Fit::ParVec *pars) const;
  void apply_2f_mod(PrEW::Connect::DataConnector *connector,
                    PrEW::Fit::ParVec *pars) const;

  void add_to_connector(PrEW::Connect::DataConnector *connector,
                        const PrEW::Data::CoefDistrVec &coef_distrs,
                        const PrEW::Data::PredLinkVec &pred_links) const;

  void order_pars(PrEW::Fit::ParVec *pars) const;

  void print_result(PrEW::Connect::DataConnector *connector,
                    PrEW::Fit::ParVec *pars) const;
};

} // namespace Setups
} // namespace PrEWUtils

#endif