#ifndef OCEANWAVESFILLINFLOW_H
#define OCEANWAVESFILLINFLOW_H

#include <AMReX_Array.H>
#include <AMReX_BCRec.H>
#include <AMReX_Box.H>
#include <AMReX_BoxArray.H>
#include <AMReX_FabArray.H>
#include <AMReX_FabArrayCommI.H>
#include <AMReX_FillPatchUtil_I.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <AMReX_IntVect.H>
#include <AMReX_Loop.H>
#include <AMReX_REAL.H>

#include "amr-wind/core/FieldFillPatchOps.H"
#include "amr-wind/core/FieldBCOps.H"
#include "amr-wind/ocean_waves/boundary_ops/OceanWavesBoundary.H"
#include "amr-wind/core/FieldDescTypes.H"

namespace amrex {
class AmrCore;
} // namespace amrex

namespace amr_wind {
class Field;
class OceanWavesBoundary;
class SimTime;

class OceanWavesFillInflow : public FieldFillPatchOps<FieldBCDirichlet>
{
public:
    OceanWavesFillInflow(
        Field& field,
        const amrex::AmrCore& mesh,
        const SimTime& time,
        const OceanWavesBoundary& ow_bndry);

    ~OceanWavesFillInflow() override;

    void fillpatch(
        int lev,
        amrex::Real time,
        amrex::MultiFab& mfab,
        const amrex::IntVect& nghost,
        FieldState fstate = FieldState::New) override;

    void fillpatch_sibling_fields(
        int lev,
        amrex::Real time,
        amrex::Array<amrex::MultiFab*, AMREX_SPACEDIM>& mfabs,
        amrex::Array<amrex::MultiFab*, AMREX_SPACEDIM>& ffabs,
        amrex::Array<amrex::MultiFab*, AMREX_SPACEDIM>& cfabs,
        const amrex::IntVect& nghost,
        const amrex::Vector<amrex::BCRec>& bcrec,
        const amrex::Vector<amrex::BCRec>& /* unused */,
        FieldState fstate = FieldState::New) override;

    //! Implementation that handles filling patches from a coarse to fine level
    void fillpatch_from_coarse(
        int lev,
        amrex::Real time,
        amrex::MultiFab& mfab,
        const amrex::IntVect& nghost,
        FieldState fstate = FieldState::New) override;

    //! Implementation that handles filling physical boundary conditions
    void fillphysbc(
        int lev,
        amrex::Real time,
        amrex::MultiFab& mfab,
        const amrex::IntVect& nghost,
        FieldState fstate = FieldState::New) override;

protected:
    const OceanWavesBoundary& m_ow_bndry;
};

} // namespace amr_wind

#endif /* ABLFILLINFLOW_H */
