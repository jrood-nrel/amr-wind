#ifndef LINEAR_WAVES_OPS_H
#define LINEAR_WAVES_OPS_H

#include "amr-wind/physics/multiphase/MultiPhase.H"
#include "amr-wind/ocean_waves/relaxation_zones/LinearWaves.H"
#include "amr-wind/ocean_waves/OceanWavesOps.H"
#include "amr-wind/ocean_waves/OceanWaves.H"
#include "amr-wind/ocean_waves/relaxation_zones/relaxation_zones_ops.H"
#include "amr-wind/equation_systems/vof/volume_fractions.H"

namespace amr_wind {
namespace ocean_waves {

namespace ops {

template <>
struct ReadInputsOp<LinearWaves>
{
    void operator()(
        LinearWaves::DataType& data, const ::amr_wind::utils::MultiParser& pp)
    {
        auto& wdata = data.meta();
        auto& info = data.info();
        relaxation_zones::read_inputs(wdata, info, pp);

        pp.get("wave_length", wdata.wave_length);
        pp.get("wave_height", wdata.wave_height);
    }
};

template <>
struct InitDataOp<LinearWaves>
{
    void operator()(LinearWaves::DataType& data)
    {
        const auto& wdata = data.meta();

        auto& sim = data.sim();
        // cppcheck-suppress constVariable
        auto& m_levelset = sim.repo().get_field("levelset");
        // cppcheck-suppress constVariable
        auto& velocity = sim.repo().get_field("velocity");
        auto& vof = sim.repo().get_field("vof");
        // cppcheck-suppress constVariable
        auto& density = sim.repo().get_field("density");
        // use MultiPhase information
        const auto& mphase = sim.physics_manager().get<MultiPhase>();
        const amrex::Real rho1 = mphase.rho1();
        const amrex::Real rho2 = mphase.rho2();

        auto nlevels = sim.repo().num_active_levels();
        auto geom = sim.mesh().Geom();
        for (int lev = 0; lev < nlevels; ++lev) {
            const auto& problo = geom[lev].ProbLoArray();
            const auto& dx = geom[lev].CellSizeArray();
            for (amrex::MFIter mfi(m_levelset(lev)); mfi.isValid(); ++mfi) {

                auto phi = m_levelset(lev).array(mfi);
                auto vel = velocity(lev).array(mfi);
                auto volfrac = vof(lev).array(mfi);
                auto rho = density(lev).array(mfi);

                const amrex::Real zsl = wdata.zsl;
                const auto& gbx3 = mfi.growntilebox(3);

                if (wdata.init_wave_field) {
                    const amrex::Real waveheight = wdata.wave_height;
                    const amrex::Real wavelength = wdata.wave_length;
                    const amrex::Real waterdepth = wdata.water_depth;
                    amrex::ParallelFor(
                        gbx3,
                        [=] AMREX_GPU_DEVICE(int i, int j, int k) noexcept {
                            const amrex::Real xc =
                                problo[0] + (i + 0.5) * dx[0];
                            const amrex::Real zc =
                                problo[2] + (k + 0.5) * dx[2];

                            const amrex::Real wavenumber =
                                2. * M_PI / wavelength;
                            const amrex::Real omega = std::pow(
                                wavenumber * 9.81 *
                                    std::tanh(wavenumber * waterdepth),
                                0.5);
                            const amrex::Real phase = wavenumber * xc;
                            const amrex::Real eta =
                                waveheight / 2.0 * std::cos(phase);

                            phi(i, j, k) = eta - zc;

                            if (phi(i, j, k) >= 0) {
                                vel(i, j, k, 0) =
                                    omega * waveheight / 2.0 *
                                    std::cosh(wavenumber * (zc + waterdepth)) /
                                    std::sinh(wavenumber * waterdepth) *
                                    std::cos(phase);
                                vel(i, j, k, 1) = 0.0;
                                vel(i, j, k, 2) =
                                    omega * waveheight / 2.0 *
                                    std::sinh(wavenumber * (zc + waterdepth)) /
                                    std::sinh(wavenumber * waterdepth) *
                                    std::sin(phase);
                            }
                        });

                } else {
                    amrex::ParallelFor(
                        gbx3,
                        [=] AMREX_GPU_DEVICE(int i, int j, int k) noexcept {
                            const amrex::Real z = problo[2] + (k + 0.5) * dx[2];
                            phi(i, j, k) = zsl - z;
                            vel(i, j, k, 0) = 0.0;
                            vel(i, j, k, 1) = 0.0;
                            vel(i, j, k, 2) = 0.0;
                        });
                }
                // Translate levelset to vof
                const amrex::Real eps = 2. * std::cbrt(dx[0] * dx[1] * dx[2]);
                const auto& gbx2 = mfi.growntilebox(2);
                amrex::ParallelFor(
                    gbx2, [=] AMREX_GPU_DEVICE(int i, int j, int k) noexcept {
                        volfrac(i, j, k) =
                            multiphase::levelset_to_vof(i, j, k, eps, phi);
                        // Make sure that density is updated before entering the
                        // solution
                        rho(i, j, k) = rho1 * volfrac(i, j, k) +
                                       rho2 * (1. - volfrac(i, j, k));
                    });
            }
        }
        vof.fillpatch(0.0);
    }
};

template <>
struct UpdateRelaxZonesOp<LinearWaves>
{
    void operator()(LinearWaves::DataType& data)
    {
        const auto& wdata = data.meta();

        auto& sim = data.sim();

        const auto& time = sim.time().new_time();
        // cppcheck-suppress constVariable
        auto& m_ow_levelset = sim.repo().get_field("ow_levelset");
        // cppcheck-suppress constVariable
        auto& m_ow_velocity = sim.repo().get_field("ow_velocity");

        auto nlevels = sim.repo().num_active_levels();
        auto geom = sim.mesh().Geom();

        for (int lev = 0; lev < nlevels; ++lev) {
            const auto& problo = geom[lev].ProbLoArray();
            // const auto& probhi = geom[lev].ProbHiArray();
            const auto& dx = geom[lev].CellSizeArray();

            for (amrex::MFIter mfi(m_ow_levelset(lev)); mfi.isValid(); ++mfi) {
                auto phi = m_ow_levelset(lev).array(mfi);
                auto vel = m_ow_velocity(lev).array(mfi);

                const amrex::Real waveheight = wdata.wave_height;
                const amrex::Real wavelength = wdata.wave_length;
                const amrex::Real waterdepth = wdata.water_depth;

                const auto& gbx = mfi.growntilebox(3);
                amrex::ParallelFor(
                    gbx, [=] AMREX_GPU_DEVICE(int i, int j, int k) noexcept {
                        const amrex::Real xc = problo[0] + (i + 0.5) * dx[0];
                        const amrex::Real zc = problo[2] + (k + 0.5) * dx[2];

                        const amrex::Real wavenumber = 2. * M_PI / wavelength;
                        const amrex::Real omega = std::pow(
                            wavenumber * 9.81 *
                                std::tanh(wavenumber * waterdepth),
                            0.5);
                        const amrex::Real phase =
                            wavenumber * xc - omega * time;

                        const amrex::Real eta =
                            waveheight / 2.0 * std::cos(phase);

                        phi(i, j, k) = eta - zc;

                        if (phi(i, j, k) >= 0) {
                            vel(i, j, k, 0) =
                                omega * waveheight / 2.0 *
                                std::cosh(wavenumber * (zc + waterdepth)) /
                                std::sinh(wavenumber * waterdepth) *
                                std::cos(phase);
                            vel(i, j, k, 1) = 0.0;
                            vel(i, j, k, 2) =
                                omega * waveheight / 2.0 *
                                std::sinh(wavenumber * (zc + waterdepth)) /
                                std::sinh(wavenumber * waterdepth) *
                                std::sin(phase);
                        }
                    });
            }
        }
    }
};

} // namespace ops
} // namespace ocean_waves
} // namespace amr_wind

#endif /* LINEAR_WAVES_OPS_H */