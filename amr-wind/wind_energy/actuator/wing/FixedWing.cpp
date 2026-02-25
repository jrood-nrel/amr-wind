#include "amr-wind/wind_energy/actuator/wing/FixedWing.H"

#include <AMReX_Box.H>
#include <AMReX_GpuContainers.H>
#include <AMReX_GpuDevice.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <set>

#include "amr-wind/wind_energy/actuator/wing/fixed_wing_ops.H"
#include "amr-wind/wind_energy/actuator/ActuatorModel.H"
#include "amr-wind/core/Slice.H"
#include "amr-wind/core/vs/vectorI.H"
#include "amr-wind/wind_energy/actuator/FLLC.H"
#include "amr-wind/wind_energy/actuator/actuator_opsI.H"

namespace amr_wind::actuator {

template class ActModel<FixedWing, ActSrcLine>;

} // namespace amr_wind::actuator
