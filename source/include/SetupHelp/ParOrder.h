#ifndef LIB_PARORDER_H
#define LIB_PARORDER_H 1

// includes from PrEW
#include <Fit/FitPar.h>

// Standard library
#include <map>
#include <string>
#include <vector>

namespace PrEWUtils {
namespace SetupHelp {

namespace ParOrder {
using IDVec = std::vector<std::string>;
using IDMap = std::map<std::string, IDVec>;
using Ordering = std::vector<std::string>;
using CategorizedPars = std::map<std::string, PrEW::Fit::ParVec>;

// Map from "common name" to identifiers
static const IDMap default_par_map{
    {"Lumi", {"Lumi"}},
    {"Pols", {"ePol", "pPol"}},
    {"AccBoxes",
     {"Acceptance_center", "Acceptance_width", "_dCenter", "_dWidth"}},
    {"Efficiencies", {"ConstEff"}},
    {"TGCs", {"Delta-g1Z", "Delta-kappa_gamma", "Delta-lambda_gamma"}},
    {"Asymmetries", {"DeltaA"}},
    {"2f_scale", {"s0_"}},
    {"2f_shape", {"Ae_", "Af_", "ef_", "AFB_", "kL_", "kR_", "k0_", "dk_"}},
    {"XSectionScalings", {"ScaleTotChiXS"}}};

static const Ordering default_ordering{
  "Lumi", "Pols", // Machine
  "TGCs", "Asymmetries", "2f_shape", // Shape effects
  "2f_scale", "XSectionScalings", // Scale effect
  "Efficiencies", "AccBoxes" // Other systematics
};

bool par_fits_ID(const PrEW::Fit::FitPar &par, const IDVec &ids);

CategorizedPars categorize_pars(const PrEW::Fit::ParVec &pars,
                                const IDMap &id_map = default_par_map);

PrEW::Fit::ParVec reorder_pars(const PrEW::Fit::ParVec &pars,
                               const Ordering &ordering = default_ordering,
                               const IDMap &id_map = default_par_map);

} // namespace ParOrder

} // namespace SetupHelp
} // namespace PrEWUtils

#endif
