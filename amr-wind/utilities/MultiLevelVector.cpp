#include "amr-wind/utilities/MultiLevelVector.H"

#include <AMReX_BLassert.H>
#include <AMReX_Box.H>
#include <AMReX_FArrayBox.H>
#include <AMReX_FabArray.H>
#include <AMReX_Geometry.H>
#include <AMReX_GpuContainers.H>
#include <AMReX_GpuDevice.H>
#include <AMReX_GpuQualifiers.H>
#include <AMReX_IntVect.H>
#include <AMReX_MFParallelFor.H>
#include <AMReX_MultiFab.H>
#include <AMReX_PODVector.H>
#include <AMReX_Vector.H>
#include <utility>

#include "amr-wind/core/Field.H"
#include "amr-wind/core/FieldRepo.H"

namespace amr_wind {

void MultiLevelVector::resize(
    const int axis, const amrex::Vector<amrex::Geometry>& geom)
{
    const auto nlevels = geom.size();
    m_axis = axis;
    m_data_h.resize(nlevels);
    m_data_d.resize(nlevels);
    for (int lev = 0; lev < nlevels; ++lev) {

        const int nc = geom[lev].Domain().length()[axis] +
                       ((m_floc == FieldLoc::CELL) ? 0 : 1);
        m_data_h[lev].resize(nc);
        m_data_h[lev].assign(nc, 0);
        m_data_d[lev].resize(nc);
    }
    copy_host_to_device();
}

void MultiLevelVector::copy_host_to_device()
{
    for (int lev = 0; lev < m_data_h.size(); ++lev) {
        amrex::Gpu::copyAsync(
            amrex::Gpu::hostToDevice, m_data_h[lev].begin(),
            m_data_h[lev].end(), m_data_d[lev].begin());
    }
}

void MultiLevelVector::copy_to_field(Field& fld)
{
    AMREX_ALWAYS_ASSERT(fld.repo().num_active_levels() == m_data_h.size());
    AMREX_ALWAYS_ASSERT(fld.num_comp() == 1);
    for (int lev = 0; lev < m_data_h.size(); ++lev) {
        auto const& farrs = fld(lev).arrays();
        const amrex::IntVect ngs(0);
        const auto* d_ptr = m_data_d[lev].data();
        const int axis = m_axis;
        amrex::ParallelFor(
            fld(lev), ngs,
            [=] AMREX_GPU_DEVICE(int nbx, int i, int j, int k) noexcept {
                const int idx = (axis == 0) ? i : ((axis == 1) ? j : k);
                farrs[nbx](i, j, k, 0) = d_ptr[idx];
            });
    }
    amrex::Gpu::streamSynchronize();
}
} // namespace amr_wind
