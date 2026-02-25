#ifndef BOX_H
#define BOX_H

#include <AMReX_REAL.H>
#include <string>

#include "amr-wind/immersed_boundary/bluff_body/BluffBody.H"
#include "amr-wind/core/vs/vector.H"
#include "amr-wind/immersed_boundary/IBTypes.H"

namespace amr_wind::ib {

struct BoxData : public BluffBodyBaseData
{
    vs::Vector center_loc{0.0_rt, 0.0_rt, 0.0_rt};
    amrex::Real length{0.1_rt};
    amrex::Real width{0.1_rt};
    amrex::Real height{0.1_rt};
};

struct Box : public BluffBodyType
{
    using InfoType = IBInfo;
    using MetaType = BoxData;
    using DataType = IBDataHolder<Box>;

    static std::string identifier() { return "Box"; }
};

} // namespace amr_wind::ib

#endif /* BOX_H */
