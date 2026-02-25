#include <AMReX_Box.H>
#include <AMReX_GpuContainers.H>
#include <AMReX_GpuDevice.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <set>
#include <type_traits>

#include "amr-wind/wind_energy/actuator/disk/UniformCt.H"
#include "amr-wind/wind_energy/actuator/disk/Joukowsky.H"
#include "amr-wind/wind_energy/actuator/ActuatorModel.H"
#include "amr-wind/core/vs/tensorI.H"
#include "amr-wind/core/vs/vectorI.H"
#include "amr-wind/wind_energy/actuator/actuator_opsI.H"
#include "amr-wind/wind_energy/actuator/actuator_types.H"

namespace amr_wind::actuator {
template class ActModel<UniformCt, ActSrcDisk>;
template class ActModel<Joukowsky, ActSrcDisk>;

} // namespace amr_wind::actuator
