#include <AMReX_Array.H>
#include <AMReX_BC_TYPES.H>
#include <AMReX_BLassert.H>
#include <AMReX_Box.H>
#include <AMReX_EBMultiFabUtil.H>
#include <AMReX_MultiFab.H>
#include <AMReX_VisMF.H>

#include <incflo_level.H>
#include <boundary_conditions_F.H>

//
// Set the BCs for all the variables EXCEPT pressure or velocity.
//
void incflo_level::incflo_set_scalar_bcs(int lev)
{
	BL_PROFILE("incflo_level::incflo_set_scalar_bcs()");

	Box domain(geom[lev].Domain());

#ifdef _OPENMP
#pragma omp parallel
#endif
	for(MFIter mfi(*ro[lev], true); mfi.isValid(); ++mfi)
	{
		set_scalar_bcs((*ro[lev])[mfi].dataPtr(),
					   (*mu[lev])[mfi].dataPtr(),
					   BL_TO_FORTRAN_ANYD((*lambda[lev])[mfi]),
					   bc_ilo.dataPtr(),
					   bc_ihi.dataPtr(),
					   bc_jlo.dataPtr(),
					   bc_jhi.dataPtr(),
					   bc_klo.dataPtr(),
					   bc_khi.dataPtr(),
					   domain.loVect(),
					   domain.hiVect(),
					   &nghost);
	}
	ro[lev]->FillBoundary(geom[lev].periodicity());
	mu[lev]->FillBoundary(geom[lev].periodicity());
	lambda[lev]->FillBoundary(geom[lev].periodicity());
}

//
// Set the BCs for velocity only
//
void incflo_level::incflo_set_velocity_bcs(int lev, int extrap_dir_bcs)
{
	BL_PROFILE("incflo_level::incflo_set_velocity_bcs()");

	vel[lev]->FillBoundary(geom[lev].periodicity());

	Box domain(geom[lev].Domain());

#ifdef _OPENMP
#pragma omp parallel
#endif
	for(MFIter mfi(*vel[lev], true); mfi.isValid(); ++mfi)
	{
		set_velocity_bcs(BL_TO_FORTRAN_ANYD((*vel[lev])[mfi]),
						 bc_ilo.dataPtr(),
						 bc_ihi.dataPtr(),
						 bc_jlo.dataPtr(),
						 bc_jhi.dataPtr(),
						 bc_klo.dataPtr(),
						 bc_khi.dataPtr(),
						 domain.loVect(),
						 domain.hiVect(),
						 &nghost,
						 &extrap_dir_bcs);
	}
}

//
// Fills ghost cell values of pressure appropriately for the BC type
//
void incflo_level::incflo_extrap_pressure(int lev, std::unique_ptr<amrex::MultiFab>& p)
{
	BL_PROFILE("incflo_level::incflo_extrap_pressure()");

	Box domain(geom[lev].Domain());

#ifdef _OPENMP
#pragma omp parallel
#endif
	for(MFIter mfi(*p, true); mfi.isValid(); ++mfi)
	{

		extrap_pressure_to_ghost_cells(BL_TO_FORTRAN_ANYD((*p)[mfi]),
									   bc_ilo.dataPtr(),
									   bc_ihi.dataPtr(),
									   bc_jlo.dataPtr(),
									   bc_jhi.dataPtr(),
									   bc_klo.dataPtr(),
									   bc_khi.dataPtr(),
									   domain.loVect(),
									   domain.hiVect(),
									   &nghost);
	}
}

void incflo_level::fill_mf_bc(int lev, MultiFab& mf)
{
	Box domain(geom[lev].Domain());

	if(!mf.boxArray().ixType().cellCentered())
		amrex::Error("fill_mf_bc only used for cell-centered arrays!");

	// Impose periodic bc's at domain boundaries and fine-fine copies in the interior
	mf.FillBoundary(geom[lev].periodicity());

// Fill all cell-centered arrays with first-order extrapolation at domain boundaries
#ifdef _OPENMP
#pragma omp parallel
#endif
	for(MFIter mfi(mf, true); mfi.isValid(); ++mfi)
	{
		const Box& sbx = mf[mfi].box();
		fill_bc0(mf[mfi].dataPtr(),
				 sbx.loVect(),
				 sbx.hiVect(),
				 bc_ilo.dataPtr(),
				 bc_ihi.dataPtr(),
				 bc_jlo.dataPtr(),
				 bc_jhi.dataPtr(),
				 bc_klo.dataPtr(),
				 bc_khi.dataPtr(),
				 domain.loVect(),
				 domain.hiVect(),
				 &nghost);
	}
}

