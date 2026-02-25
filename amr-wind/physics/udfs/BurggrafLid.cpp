#include "amr-wind/physics/udfs/BurggrafLid.H"

#include <AMReX_BLassert.H>

#include "amr-wind/core/Field.H"
#include "amr-wind/equation_systems/icns/icns.H"

namespace amr_wind::udf {

BurggrafLid::BurggrafLid(const Field& fld)
{
    AMREX_ALWAYS_ASSERT(fld.name() == pde::ICNS::var_name());
    AMREX_ALWAYS_ASSERT(fld.num_comp() == AMREX_SPACEDIM);
}

} // namespace amr_wind::udf
