#include <AMReX_FArrayBox.H>
#include <AMReX_GpuLaunchFunctsC.H>
#include <AMReX_GpuQualifiers.H>
#include <AMReX_MultiFab.H>
#include <AMReX_ParmParse.H>

#include "amr-wind/equation_systems/sdr/source_terms/SDRSrc.H"
#include "amr-wind/CFDSim.H"
#include "AMReX_REAL.H"
#include "amr-wind/core/Field.H"
#include "amr-wind/core/FieldDescTypes.H"
#include "amr-wind/core/FieldRepo.H"

using namespace amrex::literals;

namespace amr_wind::pde::tke {

SDRSrc::SDRSrc(const CFDSim& sim)
    : m_sdr_src(sim.repo().get_field("omega_src"))
    , m_sdr_diss(sim.repo().get_field("sdr_dissipation"))
{}

SDRSrc::~SDRSrc() = default;

void SDRSrc::operator()(
    const int lev,
    const amrex::MFIter& mfi,
    const amrex::Box& bx,
    const FieldState fstate,
    const amrex::Array4<amrex::Real>& src_term) const
{
    const auto& sdr_src_arr = (this->m_sdr_src)(lev).array(mfi);
    const auto& sdr_diss_arr = (this->m_sdr_diss)(lev).array(mfi);

    const amrex::Real factor = (fstate == FieldState::NPH) ? 0.5_rt : 1.0_rt;

    amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k) noexcept {
        src_term(i, j, k) +=
            (factor * sdr_diss_arr(i, j, k)) + sdr_src_arr(i, j, k);
    });
}

} // namespace amr_wind::pde::tke
