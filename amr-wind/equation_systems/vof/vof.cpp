#include "amr-wind/equation_systems/vof/vof.H"

#include <AMReX_Box.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <AMReX_MFParallelFor.H>

#include "amr-wind/equation_systems/PDE.H"
#include "amr-wind/equation_systems/PDEHelpers.H"
#include "amr-wind/equation_systems/SchemeTraits.H"

namespace amr_wind::pde {

template class PDESystem<VOF, fvm::Godunov>;
template class PDESystem<VOF, fvm::MOL>;

} // namespace amr_wind::pde
