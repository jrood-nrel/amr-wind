#include "amr-wind/equation_systems/passive_scalar/passive_scalar.H"

#include <AMReX_Box.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <AMReX_MFParallelFor.H>

#include "amr-wind/equation_systems/PDE.H"
#include "amr-wind/equation_systems/SchemeTraits.H"

namespace amr_wind::pde {

template class PDESystem<PassiveScalar, fvm::Godunov>;
template class PDESystem<PassiveScalar, fvm::MOL>;

} // namespace amr_wind::pde
