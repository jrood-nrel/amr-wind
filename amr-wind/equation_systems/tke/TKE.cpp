#include "amr-wind/equation_systems/tke/TKE.H"

#include <AMReX_Box.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <AMReX_MFParallelFor.H>

#include "amr-wind/core/FieldDescTypes.H"
#include "amr-wind/equation_systems/PDE.H"
#include "amr-wind/equation_systems/PDEHelpers.H"
#include "amr-wind/equation_systems/SchemeTraits.H"

namespace amr_wind::pde {

template class PDESystem<TKE, fvm::Godunov>;
template class PDESystem<TKE, fvm::MOL>;

} // namespace amr_wind::pde
