#include "amr-wind/equation_systems/icns/icns.H"

#include <AMReX_MFParallelFor.H>

#include "amr-wind/convection/Godunov.H"
#include "amr-wind/equation_systems/PDE.H"
#include "amr-wind/equation_systems/SchemeTraits.H"

namespace amr_wind::pde {

template class PDESystem<ICNS, ::amr_wind::fvm::Godunov>;
template class PDESystem<ICNS, ::amr_wind::fvm::MOL>;

} // namespace amr_wind::pde
