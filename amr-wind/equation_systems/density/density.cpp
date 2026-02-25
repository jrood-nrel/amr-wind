#include "amr-wind/equation_systems/density/density.H"

#include <AMReX_Box.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <AMReX_MFParallelFor.H>

#include "amr-wind/equation_systems/PDE.H"
#include "amr-wind/equation_systems/SchemeTraits.H"

namespace amr_wind::pde {

template class PDESystem<Density, fvm::Godunov>;
template class PDESystem<Density, fvm::MOL>;

} // namespace amr_wind::pde
