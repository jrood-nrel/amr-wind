#include <fstream>

#include "amr-wind/utilities/io_utils.H"

namespace amr_wind::ioutils {

void read_flat_grid_file(
    const std::string& fname,
    amrex::Vector<amrex::Real>& xs,
    amrex::Vector<amrex::Real>& ys,
    amrex::Vector<amrex::Real>& zs)
{
    std::ifstream file(fname, std::ios::in);

    if (!file.good()) {
        amrex::Abort("Cannot find file");
    }

    size_t nx = 0;
    size_t ny = 0;
    file >> nx;
    file >> ny;
    AMREX_ALWAYS_ASSERT(nx > 0);
    AMREX_ALWAYS_ASSERT(ny > 0);
    xs.resize(nx);
    ys.resize(ny);
    zs.resize(nx * ny);
    for (size_t n = 0; n < nx; n++) {
        file >> xs[n];
    }
    for (size_t n = 0; n < ny; n++) {
        file >> ys[n];
    }
    for (size_t n = 0; n < nx * ny; n++) {
        file >> zs[n];
    }
    file.close();
}
} // namespace amr_wind::ioutils