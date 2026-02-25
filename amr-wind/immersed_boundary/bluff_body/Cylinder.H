#ifndef CYLINDER_H
#define CYLINDER_H

#include <string>

#include "amr-wind/immersed_boundary/bluff_body/BluffBody.H"
#include "AMReX_REAL.H"
#include "amr-wind/core/vs/vector.H"
#include "amr-wind/immersed_boundary/IBTypes.H"

using namespace amrex::literals;

namespace amr_wind::ib {

struct CylinderData : public BluffBodyBaseData
{
    vs::Vector center_loc{0.0_rt, 0.0_rt, 0.0_rt};
    amrex::Real radius{0.1_rt};
    amrex::Real height{1.0_rt};
};

struct Cylinder : public BluffBodyType
{
    using InfoType = IBInfo;
    using MetaType = CylinderData;
    using DataType = IBDataHolder<Cylinder>;

    static std::string identifier() { return "Cylinder"; }
};

} // namespace amr_wind::ib

#endif /* CYLINDER_H */
