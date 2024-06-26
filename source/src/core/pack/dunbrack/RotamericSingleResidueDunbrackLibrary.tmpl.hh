// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/scoring/dunbrack/RotamericSingleResidueDunbrackLibrary.tmpl.hh
/// @brief   Declaration of rotameric rotamer libraries from Jun08 (as opposed to semi-rotameric)
/// @author  Andrew Leaver-Fay
/// @author Vikram K. Mulligan (vmullig@uw.edu) -- Added Voronoi method for noncanonical fa_dun scoring.


#ifndef INCLUDED_core_pack_dunbrack_RotamericSingleResidueDunbrackLibrary_tmpl_hh
#define INCLUDED_core_pack_dunbrack_RotamericSingleResidueDunbrackLibrary_tmpl_hh

#if (defined WIN32) && (!defined WIN_PYROSETTA)
#define ZLIB_WINAPI  // REQUIRED FOR WINDOWS
#endif

// Unit Headers
#include <core/pack/dunbrack/RotamericSingleResidueDunbrackLibrary.hh>
#include <core/pack/dunbrack/RotamericSingleResidueDunbrackLibraryParser.hh>
#include <core/pack/dunbrack/RotamericSingleResidueDunbrackLibraryParser.tmpl.hh>

// Package Headers
#include <core/id/PartialAtomID.hh>
#include <core/pack/dunbrack/RotamerLibrary.hh>
#include <core/pack/dunbrack/RotamerLibraryScratchSpace.hh>
#include <core/pack/dunbrack/DunbrackRotamer.hh>
#include <core/pack/dunbrack/ChiSet.hh>
#include <core/pack/dunbrack/SingleResidueDunbrackLibrary.hh>
#include <core/pack/rotamers/SingleResidueRotamerLibrary.fwd.hh>
#include <core/pack/dunbrack/PeptoidDOFReporters.hh> // DO NOT AUTO-REMOVE needed for proper OP conversion
#include <core/pack/dunbrack/StandardDOFReporters.hh> // DO NOT AUTO-REMOVE needed for proper OP conversion

// Project Headers
#include <utility/graph/Graph.fwd.hh>
#include <core/pack/task/PackerTask.hh>
#include <core/pack/rotamer_set/RotamerSetOperation.hh>
#include <core/pack/rotamer_set/RotamerSetOperation.fwd.hh>
#include <core/pack/task/RotamerSampleOptions.hh>
#include <core/pack/task/ResidueLevelTask.hh>
#include <core/id/TorsionID.hh>

#include <core/pose/Pose.hh>

#include <basic/basic.hh>
#include <basic/Tracer.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/corrections.OptionKeys.gen.hh>

// Boost Headers

//Auto Headers
#include <core/types.hh>
#include <core/chemical/rings/RingConformer.hh>
#include <core/chemical/rings/RingConformerSet.hh>
#include <core/chemical/ResidueType.hh>
#include <core/conformation/Residue.hh>
#include <core/chemical/rotamers/NCAARotamerLibrarySpecification.hh>
#include <core/conformation/Residue.fwd.hh>
#include <core/conformation/ResidueFactory.hh>

#include <core/scoring/ScoreFunction.fwd.hh>


// Utility Headers
#include <utility/exit.hh>
#include <utility/LexicographicalIterator.hh>
#include <utility/fixedsizearray1.hh>
#include <utility/vector1.hh>
#include <utility/vectorL.hh>
#include <utility/io/izstream.hh>
#include <utility/io/ozstream.hh>

#include <numeric/types.hh>
#include <numeric/random/random.hh>
#include <numeric/xyz.functions.hh>
#include <numeric/MathMatrix.hh>
#include <numeric/MathTensor.hh>
#include <numeric/MathNTensor.hh>
#include <numeric/interpolation/spline/CubicSpline.fwd.hh>
#include <numeric/interpolation/spline/BicubicSpline.hh>
#include <numeric/interpolation/spline/TricubicSpline.hh>
#include <numeric/util.hh>
#include <numeric/angle.functions.hh>

#include <ObjexxFCL/FArray2D.hh>

#include <algorithm>
#include <cmath>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <numeric/interpolation/spline/PolycubicSpline.hh> // AUTO IWYU For PolycubicSpline

namespace core {
namespace pack {
namespace dunbrack {

template < Size T, Size N >
RotamericSingleResidueDunbrackLibrary< T, N >::RotamericSingleResidueDunbrackLibrary(
	chemical::ResidueType const & rt,
	bool dun02,
	bool use_bicubic,
	bool dun_entropy_correction,
	bool const reduced_resolution_library
) :
	parent( rt, T, dun02, use_bicubic, dun_entropy_correction ),
	peptoid_( rt.is_peptoid() ),
	peptoid_is_achiral_( basic::options::option[ basic::options::OptionKeys::corrections::score::fa_dun_correct_achiral_peptoid_libraries ]() && rt.is_peptoid() && rt.is_achiral_sidechain() ),
	canonical_aa_( rt.is_canonical_aa() && core::chemical::is_canonical_L_aa_or_gly( rt.aa() ) ),
	canonicals_use_voronoi_( basic::options::option[ basic::options::OptionKeys::corrections::score::fa_dun_canonicals_use_voronoi ]() ),
	noncanonicals_use_voronoi_( basic::options::option[ basic::options::OptionKeys::corrections::score::fa_dun_noncanonicals_use_voronoi ]() ),
	correct_rotamer_well_order_on_read_( basic::options::option[ basic::options::OptionKeys::corrections::score::fa_dun_correct_rotamer_well_order ]() )
{
	// This is horrible but temporarily necessarily until we figure out how to distribute full beta rotlibs
	// Betas automatically (if 3 BB) have phipsi binrange of 30

	// AMW: I'm putting the is_peptoid check first just in case.
	// VKM: OK.
	if ( rt.is_peptoid() ) {
		N_BB_BINS[ 1 ] = 14;
		BB_BINRANGE[ 1 ] = 10;
		for ( Size i = 2; i <= N; ++i ) {
			N_BB_BINS[ i ] = 36;
			BB_BINRANGE[ i ] = 360.0 / N_BB_BINS[ i ];
		}
	} else {
		if ( reduced_resolution_library ) {
			for ( Size i = 1; i <= N; ++i ) {
				N_BB_BINS[ i ] = 12;
				BB_BINRANGE[ i ] = 360.0 / N_BB_BINS[ i ];
			}
		} else {
			for ( Size i = 1; i <= N; ++i ) {
				N_BB_BINS[ i ] = 36;
				BB_BINRANGE[ i ] = 360.0 / N_BB_BINS[ i ];
			}
		}
	}

	if ( rt.is_peptoid() ) {
		iv_reporters_[1] = utility::pointer::make_shared< PeptoidOmegaReporter >( parent::PEPTOID_NEUTRAL_OMG );
		for ( Size ii = 2; ii <= N; ++ii ) {
			iv_reporters_[ii] = utility::pointer::make_shared< MainchainTorsionReporter >(
				ii - 1,
				N - 1,
				( ii == 2 ? parent::PEPTOID_NEUTRAL_PHI : parent::PEPTOID_NEUTRAL_PSI ),
				false,
				false
			);
		}
	} else if ( rt.is_aramid() ) {

		if ( N > 1 ) {
			// 'first and last' are phi and psi, until we make things better.
			// these are really pretty bb-independent!
			// This logic sets your IVs by default.

			// record which mainchain torsions to use for each type of amarid
			// and then construct the appropriate MainchainTorsionReporters for
			// the iv_reporters_ array.
			Size last_tor = 2; // set this to some other value if you have an amarid with more than 2 relevant dihedrals.
			utility::fixedsizearray1< Size, N > torsion_inds;
			Real pnphi = parent::PEPTIDE_NEUTRAL_PHI, pnpsi = parent::PEPTIDE_NEUTRAL_PSI;
			if ( rt.is_ortho_aramid() ) {
				// There are only two flexible angles and they are first and last. So N must be two,
				torsion_inds[1] = 1;
				torsion_inds[2] = 3;
			} else if ( rt.is_pre_methylene_ortho_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 4;
			} else if ( rt.is_post_methylene_ortho_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 4;
			} else if ( rt.is_pre_methylene_post_methylene_ortho_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 5;
			} else if ( rt.is_meta_aramid() ) {
				// There are only two flexible angles and they are first and last. So N must be two,
				torsion_inds[1] = 1;
				torsion_inds[2] = 4;
			} else if ( rt.is_pre_methylene_meta_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 5;
			} else if ( rt.is_post_methylene_meta_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 5;
			} else if ( rt.is_pre_methylene_post_methylene_meta_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 6;
			} else if ( rt.is_para_aramid() ) {
				// There are only two flexible angles and they are first and last. So N must be two,
				torsion_inds[1] = 1;
				torsion_inds[2] = 5;
			} else if ( rt.is_pre_methylene_para_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 6;
			} else if ( rt.is_post_methylene_para_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 6;
			} else if ( rt.is_pre_methylene_post_methylene_para_aramid() ) {
				torsion_inds[1] = 1;
				torsion_inds[2] = 7;
			}
			for ( Size ii = 1; ii <= N; ++ii ) {
				iv_reporters_[ii] = utility::pointer::make_shared< MainchainTorsionReporter >(
					torsion_inds[ii],
					torsion_inds[last_tor],
					( ii == 1 ? pnphi : pnpsi ),
					true /* flip for d_aa */,
					false );
			}
		} else {
			utility_exit_with_message( "Error: Trying to define an aramid with only one mainchain torsion!" );
		}
	} else if ( rt.is_protein() ) {
		// This logic sets your IVs by default.
		Real pnphi = parent::PEPTIDE_NEUTRAL_PHI, pnpsi = parent::PEPTIDE_NEUTRAL_PSI;
		for ( Size ii = 1; ii <= N; ++ii ) {
			iv_reporters_[ii] = utility::pointer::make_shared< PeptideTorsionReporter >(
				ii, N,
				ii == 1 ? pnphi : pnpsi,
				true /*flip neutral for mirrored*/);
		}
	}

}

template < Size T, Size N >
RotamericSingleResidueDunbrackLibrary< T, N >::~RotamericSingleResidueDunbrackLibrary() throw()
{}


template < Size T, Size N >
Real
RotamericSingleResidueDunbrackLibrary< T, N >::rotamer_energy(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	rotamers::TorsionEnergy & tenergy
) const
{
	RotamerLibraryInterpolationScratch scratch;
	Real4 chidev, chidevpen;
	core::Size packed_rotno;
	return eval_rotameric_energy( rsd, pose, scratch, chidev, chidevpen, packed_rotno, tenergy );
}


template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::rotamer_energy_deriv(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	id::TorsionID const & tor_id,
	rotamers::TorsionEnergy & tderiv
) const
{
	// Given the knowledge of the TorsionID,
	// there's probably a way to skip much of the following calculations

	RotamerLibraryScratchSpace scratch;
	/// most of the work is done in this call
	this->eval_rotameric_deriv( rsd, pose, scratch );

	//Multiplier for D-amino acids:
	const core::Real d_multiplier = rsd.type().is_mirrored_type() ? -1.0 : 1.0;

	/// sum derivatives.
	Real5 & dE_dbb(  scratch.dE_dbb() );
	Real5 & dE_dbb_dev(  scratch.dE_dbb_dev() );
	Real5 & dE_dbb_rot(  scratch.dE_dbb_rot() );
	Real4 & dE_dchi( scratch.dE_dchi() );
	Real4 & dE_dchi_dev( scratch.dE_dchi_dev() );

	// p0 - the base probability -- not modified by the chi-dev penalty
	Real const rotprob( scratch.rotprob() );
	Real const invp( ( rotprob == Real( 0.0 ) ) ? 0.0 : -1.0 / rotprob );

	for ( Size bbi = 1; bbi <= N; ++bbi ) {
		if ( use_bicubic() ) {
			dE_dbb_dev[ bbi ] = d_multiplier * scratch.dchidevpen_dbb()[ bbi ];
			dE_dbb_rot[ bbi ] = d_multiplier * scratch.dneglnrotprob_dbb()[ bbi ];
		} else {
			dE_dbb_dev[ bbi ] = d_multiplier * scratch.dchidevpen_dbb()[ bbi ];
			dE_dbb_rot[ bbi ] = d_multiplier * invp * scratch.drotprob_dbb()[ bbi ];
		}

		// Correction for entropy
		if ( dun_entropy_correction() ) {
			dE_dbb_rot[ bbi ] += d_multiplier * scratch.dentropy_dbb()[ bbi ];
		}

		dE_dbb[ bbi ]   = dE_dbb_dev[ bbi ] + dE_dbb_rot[ bbi ];
	}

	for ( Size i = 1; i <= T; ++i ) {
		dE_dchi[ i ]     = d_multiplier * scratch.dchidevpen_dchi()[ i ];
		dE_dchi_dev[ i ] = d_multiplier * scratch.dchidevpen_dchi()[ i ];
	}

	correct_termini_derivatives( rsd, scratch );

	scratch.extract_torsion_deriv( tor_id, rsd, pose, tderiv );
}

template < Size T, Size N >
std::set< id::PartialAtomID >
RotamericSingleResidueDunbrackLibrary< T, N >::atoms_w_dof_derivatives(
	conformation::Residue const & rsd,
	pose::Pose const & pose
) const
{
	std::set< id::PartialAtomID > atoms;
	for ( auto const & iv_reporter : iv_reporters_ ) {
		iv_reporter->insert_atoms_defining_dof( rsd, pose, atoms );
	}
	// TODO: append atoms difining the chi dihedrals themselves
	return atoms;

}

template < Size T, Size N >
core::Size
RotamericSingleResidueDunbrackLibrary< T, N >::get_packed_rotno(
	conformation::Residue const & rsd,
	pose::Pose const & pose
) const {
	ChiVector chi;
	return get_packed_rotno(rsd, pose, chi);
}

template < Size T, Size N >
core::Size
RotamericSingleResidueDunbrackLibrary< T, N >::get_packed_rotno(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	ChiVector & chi
) const {
	chi = rsd.chi();
	if ( rsd.type().is_mirrored_type() ) {
		for ( core::Size i(1), imax(chi.size()); i<=imax; ++i ) {
			//Invert if we're dealing with a D-amino acid.
			chi[ i ] *= -1.0;
		}
	}

	core::Size packed_rotno( 0 );

	if ( ( canonical_aa_ && canonicals_use_voronoi_ ) || ( !canonical_aa_ && noncanonicals_use_voronoi_ ) ) {
		get_rotamer_from_chi_static_voronoi( chi, packed_rotno, rsd, pose );
	} else {
		Size4 rotwell;

		// Don't use derived class's version of this function.
		RotamericSingleResidueDunbrackLibrary< T, N >::get_rotamer_from_chi_static( chi, rotwell ); //Use old logic for canonicals.
		packed_rotno = rotwell_2_packed_rotno(rotwell);

		// panic!  Extremely unlikely rotamer found.  Find another rotamer that has at least some probability,
		// and move this rotamer toward it -- do so in a predictable manner so that the score function is continuous
		// as it tries to move from this rotamer to another.
		if ( packed_rotno == 0 ) {
			packed_rotno = find_another_representative_for_unlikely_rotamer( rsd, pose, rotwell );
			//std::cout << "***new packed_rotno=" << packed_rotno << std::endl;
		}
	}

	return packed_rotno;
}

template < Size T, Size N >
Real
RotamericSingleResidueDunbrackLibrary< T, N >::eval_rotameric_energy(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	RotamerLibraryInterpolationScratch & scratch,
	Real4 & chidev,
	Real4 & chidevpen,
	core::Size & packed_rotno,
	rotamers::TorsionEnergy & tenergy
) const {
	// compute rotamer number from chi
	ChiVector chi;
	packed_rotno = get_packed_rotno( rsd, pose, chi );

	PackedDunbrackRotamer< T, N, Real > interpolated_rotamer;
	interpolate_rotamers( rsd, pose, scratch, packed_rotno, interpolated_rotamer );

	Real4 const & chimean           ( scratch.chimean()           );
	Real4 const & chisd             ( scratch.chisd()             );

	if ( dun02() && canonical_aa_ ) {
		for ( Size ii = 1; ii <= T; ++ii ) {
			chidev[ ii ] = subtract_chi_angles( chi[ ii ], chimean[ ii ], aa(), ii );
			//std::cout << "***chi[" << ii << "]=" << chi[ii] << "\tchimean[" << ii << "]=" << chimean[ii] << std::endl;
		}
	} else {
		for ( Size ii = 1; ii <= T; ++ii ) {
			chidev[ ii ] = basic::periodic_range( chi[ ii ] - chimean[ ii ], 360 );
			//std::cout << "***chi[" << ii << "]=" << chi[ii] << "\tchimean[" << ii << "]=" << chimean[ii] << std::endl;
		}
	}

	for ( Size ii = 1; ii <= T; ++ii ) {
		/// chidev penalty: define a gaussian with a height of 1 and a standard deviation of chisd[ ii ];
		/// exp( -1 * (chi_i - chi_mean_i )**2 / ( 2 * chisd_ii**2 ) )
		/// Pretend this gaussian defines a probability for having an angle at a certain deviation from the mean.
		/// Convert that probability into an energy:
		/// -ln p = -ln exp( -1* (chi_i - chi_mean_i)**2/(2 chisd_i**2) ) = (chi_i - chi_mean_i)**2/(2 chisd_i**2)
		chidevpen[ ii ] = chidev[ ii ] * chidev[ ii ] / ( 2 * chisd[ ii ] * chisd[ ii ] );

		//std::cout << "***chisd[" << ii << "]=" << chisd[ii] << "\tchidev[" << ii << "])=" << chidev[ii] << std::endl; //DELETE ME.

		/// Add in gaussian height normalization
		/// p = prod( i, 1/(stdv_i*sqrt(2*pi)) * exp( -1* (chi_i - chi_mean_i)**2/(2 chisd_i**2) ) )
		/// -ln(p) == sum( i, (chi_i - chi_mean_i)**2/(2 chisd_i**2) + log(stdv_i*sqrt(2*pi)) )
		///  == sum( i, (chi_i - chi_mean_i)**2/(2 chisd_i**2) + log(stdv_i) + log(sqrt(2*pi)))  <-- where sum(i,sqrt2pi) is just a constant per amino acid
		///     and can be treated as part of the reference energies.
		if ( basic::options::option[ basic::options::OptionKeys::corrections::score::dun_normsd ] ) {
			chidevpen[ ii ] += std::log( chisd[ ii ] );
			//std::cout << "***dun_normsd:\tchisd[" << ii << "]=" << chisd[ii] << "\tlog(chisd[" << ii << "])=" << std::log(chisd[ii]) << std::endl; //DELETE ME.
		}
	}

	Real chidevpensum( 0.0 );
	for ( Size ii = 1; ii <= T; ++ii ) chidevpensum += chidevpen[ ii ];

	core::Real rot, dev;
	if ( use_bicubic() ) {
		rot = scratch.negln_rotprob();
		dev = chidevpensum;
	} else {
		rot = -std::log(scratch.rotprob());
		dev = chidevpensum;
	}

	// Corrections for Shannon Entropy
	if ( dun_entropy_correction() ) {
		rot += scratch.entropy();
	}

	tenergy.rot += rot;
	tenergy.dev += dev;
	tenergy.tot += rot + dev;
	return rot + dev;
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::eval_rotameric_deriv(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	RotamerLibraryScratchSpace & scratch
) const
{

	Real4 & chisd             ( scratch.chisd()             );
	Real4 & chidev            ( scratch.chidev()            );
	Real4 & chidevpen         ( scratch.chidevpen()         );
	Real4 & dchidevpen_dchi   ( scratch.dchidevpen_dchi()   );
	Real5 & dchidevpen_dbb    ( scratch.dchidevpen_dbb()    );
	FiveReal4 & dchimean_dbb  ( scratch.dchimean_dbb()      );
	FiveReal4 & dchisd_dbb    ( scratch.dchisd_dbb()        );

	std::fill( chidev.begin(),            chidev.end(),            0.0 );
	std::fill( chidevpen.begin(),         chidevpen.end(),         0.0 );
	std::fill( dchidevpen_dchi.begin(),   dchidevpen_dchi.end(),   0.0 );
	std::fill( dchidevpen_dbb.begin(),    dchidevpen_dbb.end(),    0.0 );

	rotamers::TorsionEnergy tenergy;
	core::Size packed_rotno;
	eval_rotameric_energy( rsd, pose, scratch, chidev, chidevpen, packed_rotno, tenergy );

	scratch.fa_dun_tot() = tenergy.tot;
	scratch.fa_dun_rot() = tenergy.rot;
	scratch.fa_dun_semi() = tenergy.semi;
	scratch.fa_dun_dev() = tenergy.dev;

	// Now calculate the derivatives.

	for ( Size bbi = 1; bbi <= N; ++bbi ) dchidevpen_dbb[ bbi ] = 0.0;

	for ( Size ii = 1; ii <= T; ++ii ) {

		/// Backbone derivatives for chi-dev penalty.
		/// Xmean_i and sd_i both depend on phi and psi.
		/// Let: f = (X_i-Xmean_i)**2
		/// Let: g = 2 sd_i**2
		/// Then, chidevpen = f/g
		/// and, dchidevpen = (f'g - fg')/(gg)
		Real const f   = chidev[ ii ] * chidev[ ii ];
		Real const fprime = -2 * chidev[ ii ];
		Real const g   = 2 * chisd[ ii ] * chisd[ ii ];
		Real const gprime = 4 * chisd[ ii ];
		Real const invgg  = 1 / ( g * g );

		/// also need to add in derivatives for log(stdv) height normalization
		/// dE/dphi = (above) + 1/stdv * dstdv/dphi
		for ( Size bbi = 1; bbi <= N; ++bbi ) {
			scratch.dE_dbb_dev_perchi()[ bbi ][ ii ] = ( g * fprime * dchimean_dbb[ bbi ][ ii ]
				- f * gprime *   dchisd_dbb[ bbi ][ ii ] ) * invgg;

			dchidevpen_dbb[ bbi ] += scratch.dE_dbb_dev_perchi()[ bbi ][ ii ];

			// Derivatives for the change in the Gaussian height normalization due to sd changing as a function of phi
			if ( basic::options::option[ basic::options::OptionKeys::corrections::score::dun_normsd ] ) {
				dchidevpen_dbb[ bbi ]                    += 1.0 / chisd[ ii ] * dchisd_dbb[ bbi ][ ii ];
				scratch.dE_dbb_dev_perchi()[ bbi ][ ii ] += 1.0 / chisd[ ii ] * dchisd_dbb[ bbi ][ ii ];
			}
		}
		dchidevpen_dchi[ ii ] = chidev[ ii ] / ( chisd[ ii ] * chisd[ ii ] );
	}
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::assign_random_rotamer_with_bias(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	numeric::random::RandomGenerator & RG,
	ChiVector & new_chi_angles,
	bool perturb_from_rotamer_center
) const
{
	Size packed_rotno( 0 );
	assign_random_rotamer( rsd, pose, RG, new_chi_angles, perturb_from_rotamer_center, packed_rotno );
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::assign_random_rotamer(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	numeric::random::RandomGenerator & RG,
	ChiVector & new_chi_angles,
	bool perturb_from_rotamer_center,
	Size & packed_rotno
) const
{
	Real random_prob = RG.uniform();

	utility::fixedsizearray1< Real, N > const bbs( get_IVs_from_rsd( rsd, pose ) );
	utility::fixedsizearray1< Size, N > bb_bin, bb_bin_next;
	utility::fixedsizearray1< Real, N > bb_alpha;
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

	PackedDunbrackRotamer< T, N, Real > interpolated_rotamer;
	RotamerLibraryInterpolationScratch scratch;

	/// Go through rotamers in decreasing order by probability and stop when the
	Size count = 0;
	while ( random_prob > 0 ) {
		Size index = make_index< N >( N_BB_BINS, bb_bin );
		packed_rotno = rotamers_( index, ++count ).packed_rotno();
		interpolate_rotamers( scratch, packed_rotno, bb_bin, bb_bin_next, bb_alpha, interpolated_rotamer );
		random_prob -= interpolated_rotamer.rotamer_probability();
		//loop condition might end up satisfied even if we've walked through all possible rotamers
		// if the chosen random number was nearly 1
		// (and interpolation introduced a tiny bit of numerical noise).
		if ( count == rotamers_.size2() ) break;
	}
	assign_chi_for_interpolated_rotamer( interpolated_rotamer, rsd, RG, new_chi_angles, perturb_from_rotamer_center );

}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::assign_chi_for_interpolated_rotamer(
	PackedDunbrackRotamer< T, N, Real > const & interpolated_rotamer,
	conformation::Residue const & rsd,
	numeric::random::RandomGenerator & RG,
	ChiVector & new_chi_angles,
	bool perturb_from_rotamer_center
) const
{
	new_chi_angles.resize( rsd.nchi() );
	if ( ! perturb_from_rotamer_center ) {
		for ( Size ii = 1; ii <= T; ++ii ) new_chi_angles[ ii ] = interpolated_rotamer.chi_mean( ii );
	} else {
		for ( Size ii = 1; ii <= T; ++ii ) new_chi_angles[ ii ] = interpolated_rotamer.chi_mean( ii ) + RG.gaussian() * interpolated_rotamer.chi_sd(ii);
	}

	/// Set any remaining chi uniformly? ( proton chi)
	for ( Size ii = T + 1; ii <= rsd.nchi(); ++ii ) new_chi_angles[ ii ] = RG.uniform() * 360.0 - 180.0;
}


/// @details The new rotamer library represents only 75 of 81 possible arginine rotamers,
/// and 75 of 81 lysine rotamers. In the unlikely event that a rotamer is encountered
/// that's not represented in the library, find another rotamer to represent it.
/// Ideally, this stand-in rotamer would be closest to the input rotamer in physical geometry.
/// (sidechain atom rms, e.g.)
/// The following code instead first looks through all possible rotamers with a Hamming distance
/// of one from input rotamer trying to find one that works, looking first for rotamers from
/// the furthest chi toward the closest chi.  If no such rotamer may be found, it gives up and
/// returns the most-probable rotamer for a phi-psi bin.
///
/// This function modifies the "rotwell" assigned to this rotamer so that later code that relies
/// on the consistency of the rotwell and packed_rotno information will behave correctly.
template < Size T, Size N >
Size
RotamericSingleResidueDunbrackLibrary< T, N >::find_another_representative_for_unlikely_rotamer(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	Size4 & rotwell
) const
{
	/// Start from the furthest chi and work inwards trying to find another rotamer that could work
	for ( Size ii = T; ii >= 1; --ii ) {
		Size ii_orig_value = rotwell[ ii ];
		// AMW: this seems risky. If we ever have an "unlikely rotamer" situation for a residue
		// with an intermediate nonrotameric chi (imagine 4-ethyl-phenylalanine)
		// this will skip bins.
		for ( Size jj = 1; jj <= 3; ++jj ) { /// 3 == NUMBER OF ROTAMERIC CHI BINS
			if ( jj == ii_orig_value ) continue;
			rotwell[ ii ] = jj;
			Size new_packed_rotno = rotwell_2_packed_rotno( rotwell );
			if ( new_packed_rotno != 0 ) return new_packed_rotno;
		}
		/// didn't find one for chi_ii, reset rotwell to original value
		rotwell[ ii ] = ii_orig_value;
	}

	// At this point, we've examined all the rotamer wells with a Hamming distance of 1 from
	// the input rotamer well and have come up empty.
	// Just go for the most likely rotamer in this well -- no guarantee that as the rotamer swings from the
	// current rotamer well to the target rotamer well that it doesn't first swing through some other

	utility::fixedsizearray1< Real, N > bbs( get_IVs_from_rsd( rsd, pose ) );
	utility::fixedsizearray1< Size, N > bb_bin, bb_bin_next;
	utility::fixedsizearray1< Real, N > bb_alpha;
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

	Size index = make_index< N >( N_BB_BINS, bb_bin );
	Size packed_rotno = rotamers_( index, 1 ).packed_rotno();
	packed_rotno_2_rotwell( packed_rotno, rotwell );
	return packed_rotno;
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::correct_termini_derivatives(
	conformation::Residue const & rsd,
	RotamerLibraryScratchSpace & scratch
) const
{
	//Figure out indices of lower and upper torsions.
	core::Size lower_torsion_index(1), upper_torsion_index(N);

	core::chemical::rotamers::RotamerLibrarySpecificationCOP rotspec( rsd.type().rotamer_library_specification() );
	if ( rotspec!=nullptr ) {
		core::chemical::rotamers::NCAARotamerLibrarySpecificationCOP ncaa_rotspec(
			utility::pointer::dynamic_pointer_cast < core::chemical::rotamers::NCAARotamerLibrarySpecification const >( rotspec )
		);
		if ( ncaa_rotspec != nullptr ) {
			if ( ncaa_rotspec->rotamer_bb_torsion_indices().size() < N ) {
				lower_torsion_index = 0;
				upper_torsion_index = 0;
				for ( core::Size i(1), imax(ncaa_rotspec->rotamer_bb_torsion_indices().size()); i<=imax; ++i ) {
					if ( ncaa_rotspec->rotamer_bb_torsion_indices()[i] == 1 ) lower_torsion_index=i;
					if ( ncaa_rotspec->rotamer_bb_torsion_indices()[i] == N ) upper_torsion_index=i;
				}
			}
		}
	}

	// mt: for the termini, these derivatives are 0, because these "pseudo"
	// mt: torsions are kept fixed.
	// amw: assume that we are more interested in 1 and N than PHI and PSI per se; this reduces in the 1,2 case
	if ( lower_torsion_index != 0 && (rsd.is_lower_terminus() || !rsd.has_lower_connect()) ) {
		// amw: now 1 is actually better here than RotamerLibraryScratchSpace::AA_PHI_INDEX because the right element to alter is always element 1 but not necessarily has the meaning of "PHI"
		scratch.dE_dbb()[ lower_torsion_index ] = 0;
		scratch.dE_dbb_dev()[ lower_torsion_index ] = 0;
		scratch.dE_dbb_rot()[ lower_torsion_index ] = 0;
		scratch.dE_dbb_semi()[ lower_torsion_index ] = 0;
	}
	if ( upper_torsion_index != 0 && (rsd.is_upper_terminus() || !rsd.has_upper_connect()) ) {
		scratch.dE_dbb()[ upper_torsion_index ] = 0;
		scratch.dE_dbb_dev()[ upper_torsion_index ] = 0;
		scratch.dE_dbb_rot()[ upper_torsion_index ] = 0;
		scratch.dE_dbb_semi()[ upper_torsion_index ] = 0;
	}
}

/// @brief Returns the energy of the lowest-energy rotamer accessible to the given residue
/// (based on e.g. its current phi and psi values).
/// If curr_rotamer_only is true, then consider only the idealized version of the
/// residue's current rotamer (local optimum); otherwise, consider all rotamers (global optimum).
template < Size T, Size N >
Real
RotamericSingleResidueDunbrackLibrary< T, N >::best_rotamer_energy(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	bool curr_rotamer_only
) const
{
	Size const num_packed_rots = ( 1 << N );
	Real maxprob( 0 );

	if ( curr_rotamer_only ) {
		core::Size packed_rotno(0);
		if ( ( canonical_aa_ && canonicals_use_voronoi_ ) || ( !canonical_aa_ && noncanonicals_use_voronoi_ ) ) {
			get_rotamer_from_chi_static_voronoi( rsd.chi(), packed_rotno, rsd, pose );
		} else {
			Size4 rotwell;
			RotamericSingleResidueDunbrackLibrary< T, N >::get_rotamer_from_chi_static( rsd.chi(), rotwell );
			packed_rotno = rotwell_2_packed_rotno( rotwell );
		}
		PackedDunbrackRotamer< T, N, Real > interpolated_rotamer;
		RotamerLibraryInterpolationScratch scratch;
		interpolate_rotamers( rsd, pose, scratch, packed_rotno, interpolated_rotamer );
		maxprob = interpolated_rotamer.rotamer_probability();

	} else {
		// Obtain bin numbers and relative position between bins for this residue's torsions.
		utility::fixedsizearray1< Real, N > const bbs = get_IVs_from_rsd( rsd, pose );
		utility::fixedsizearray1< Size, N > bb_bin, bb_bin_next;
		utility::fixedsizearray1< Real, N > bb_alpha;
		get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

		// Determine rotamer indices for each interpolation gridpoint.
		utility::fixedsizearray1< Size, (1 << N ) > packed_rotnos;
		for ( Size indi = 1; indi <= num_packed_rots; ++indi ) {
			Size index = make_conditional_index< N >( N_BB_BINS, indi, bb_bin_next, bb_bin );
			packed_rotnos[ indi ] = rotamers_( index, 1 ).packed_rotno();
		}

		// Interpolate each packed rotamer.
		RotamerLibraryInterpolationScratch scratch;
		for ( Size ii = 1; ii <= num_packed_rots; ++ii ) {
			PackedDunbrackRotamer< T, N, Real > interpolated_rotamer;
			interpolate_rotamers( rsd, pose, scratch, packed_rotnos[ ii ], interpolated_rotamer );
			maxprob = ( maxprob < interpolated_rotamer.rotamer_probability() ?
				interpolated_rotamer.rotamer_probability() : maxprob );
		}
	}
	return -1 * std::log( maxprob );
}

/// @details The Dunbrack library's phi/psi data for a grid point
/// (x,y) collects data in the neighborhood of (x,y).  As an interpolation
/// point p moves toward a grid point (x,y), the (x,y) share in the
/// interpolated value goes to 1.  This is distinct from having interpolation wells
/// where an interpolation in the center of a well produces the maximum contribution
/// from that well.  Most of the basic::interpolation code is designed for
/// the second interpretation of interpolation, and so CTSA's Dunbrack library code did
/// funky thinks like shift by 5 degrees so that the basic::interpolation code could
/// shift it back by 5 degrees again.  The code below makes no such shift.
///
/// The alpha fraction is the distance along each axis that the interpolation point
/// has progressed from the lower grid point toward the upper grid point; it ranges
/// from 0 to 1.
template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::get_bb_bins(
	utility::fixedsizearray1< Real, N > const & bbs,
	utility::fixedsizearray1< Size, N > & bb_bin,
	utility::fixedsizearray1< Size, N > & bb_bin_next,
	utility::fixedsizearray1< Real, N > & bb_alpha
) const
{
	for ( Size ii = 1; ii <= N; ++ii ) {

		// PEPTOID SPECIAL CASING
		if ( ii == 1 && peptoid_ ) {
			if ( bbs[ ii ] >= 0.00 && bbs[ ii ] < 30 ) {
				//bin_angle( -180.0, BB_BINRANGE[ ii ], 360.0, N_BB_BINS[ ii ], numeric::principal_angle_degrees( bbs[ ii ] ), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bin_angle( -30.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::principal_angle_degrees( bbs[ ii ] ), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] += 3; bb_bin_next[ ii ] += 3;
			} else if ( bbs[ ii ] >= 30 && bbs[ ii ] < 90 ) { // [   30,   90 ) use cis_range_upper
				bin_angle( -30.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::principal_angle_degrees(30), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] += 3; bb_bin_next[ ii ] += 3;
			} else if ( bbs[ ii ] >= 90 && bbs[ ii ] < 150 ) { // [   90,  150 ) use trans_range_lower
				bin_angle( 150.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::principal_angle_degrees(150), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] += 10; bb_bin_next[ ii ] += 10;
			} else if ( bbs[ ii ] >= 150 && bbs[ ii ] <= 180.00 ) { // [  150, 180 ) use bbs[ ii ]
				bin_angle( 150.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::nonnegative_principal_angle_degrees( bbs[ ii ] ), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] += 10; bb_bin_next[ ii ] += 10;
			} else if ( bbs[ ii ] >= -180.00 && bbs[ ii ] < -150 ) { // [  180, -150 use nnpad
				bin_angle( 150.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::nonnegative_principal_angle_degrees( bbs[ ii ] ), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] -= 4; bb_bin_next[ ii ] -= 4;
			} else if ( bbs[ ii ] >= -150 && bbs[ ii ] < -90 ) { // [ -150,  -90 ) use trans_range_upper
				bin_angle( 150.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::nonnegative_principal_angle_degrees(-150), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] -= 4; bb_bin_next[ ii ] -= 4;
			} else if ( bbs[ ii ] >= -90 && bbs[ ii ] < -30 ) { // [  -90,  -30 ) use cis_range_lower
				bin_angle( -30.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::principal_angle_degrees(-30), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] += 3; bb_bin_next[ ii ] += 3;
			} else if ( bbs[ ii ] >= -30 && bbs[ ii ] < 30 ) { // [  -30,    30 ) use bbs[ ii ]
				bin_angle( -30.0, BB_BINRANGE[ ii ], 70.0, N_BB_BINS[ ii ], numeric::principal_angle_degrees( bbs[ ii ] ), bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
				bb_bin[ ii ] += 3; bb_bin_next[ ii ] += 3;
			}
			if ( bb_bin[ ii ] > 14 ) bb_bin[ ii ] -= 14;
			if ( bb_bin_next[ ii ] > 14 ) bb_bin_next[ ii ] -= 14;
			if ( bb_bin[ ii ] < 1 ) bb_bin[ ii ] += 14;
			if ( bb_bin_next[ ii ] < 1 ) bb_bin_next[ ii ] += 14;
		} else {
			parent::bin_angle( -180.0, BB_BINRANGE[ ii ], 360.0, 360.0 / BB_BINRANGE[ ii ], basic::periodic_range( bbs[ ii ], 360 ),
				bb_bin[ ii ], bb_bin_next[ ii ], bb_alpha[ ii ] );
		}
	}

	verify_bb_bins( bbs, bb_bin, bb_bin_next );
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::get_bb_bins(
	utility::fixedsizearray1< Real, N > const & bbs,
	utility::fixedsizearray1< Size, N > & bb_bin
) const
{
	utility::fixedsizearray1< Size, N > bb_bin_next;
	utility::fixedsizearray1< Real, N > bb_alpha;
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );
}

/// @brief Version for external, non-template classes to access.
/// @details Converts to fixedsizearrays internally.
/// @author Vikram K. Mulligan (vmullig@uw.edu).
template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::get_bb_bins(
	utility::vector1< core::Real > const & bbs,
	utility::vector1< core::Size > & bb_bin
) const
{
	runtime_assert_string_msg( bbs.size() == N, "Error in RotamericSingleResidueDunbrackLibrary< T, N >::get_bb_bins(): The dimensionality of the vector passed to this function was not N." );
	utility::fixedsizearray1< core::Real, N > bbs_array;
	utility::fixedsizearray1< core::Size, N > bb_bin_array;
	for ( core::Size i(1); i<=N; ++i ) bbs_array[i] = bbs[i];
	get_bb_bins(bbs_array, bb_bin_array);
	bb_bin.resize(N);
	for ( core::Size i(1); i<=N; ++i ) bb_bin[i] = bb_bin_array[i];
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::interpolate_rotamers(
	conformation::Residue const & rsd,
	pose::Pose const & pose,
	RotamerLibraryInterpolationScratch & scratch,
	Size packed_rotno,
	PackedDunbrackRotamer< T, N, Real > & interpolated_rotamer
) const
{
	utility::fixedsizearray1< Real, N > bbs = get_IVs_from_rsd( rsd, pose );
	//For D-amino acids, invert phi and psi:
	if ( rsd.type().is_mirrored_type() ) for ( Size bbi = 1; bbi <= N; ++bbi ) bbs[ bbi ] *= -1.0;

	utility::fixedsizearray1< Size, N > bb_bin, bb_bin_next;
	utility::fixedsizearray1< Real, N > bb_alpha;
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

	ChiVector chi( rsd.chi() );
	if ( rsd.type().is_mirrored_type() ) {
		for ( core::Size ii(1), iimax(chi.size()); ii<=iimax; ++ii ) chi[ii] *= -1;
	}

	interpolate_rotamers( scratch, packed_rotno, bb_bin, bb_bin_next, bb_alpha, interpolated_rotamer, chi, true );
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::interpolate_rotamers(
	RotamerLibraryInterpolationScratch & scratch,
	Size packed_rotno,
	utility::fixedsizearray1< Size, N > const & bb_bin,
	utility::fixedsizearray1< Size, N > const & bb_bin_next,
	utility::fixedsizearray1< Real, N > const & bb_alpha,
	PackedDunbrackRotamer< T, N, Real > & interpolated_rotamer,
	ChiVector const & chi/*=ChiVector(T, 0.0)*/,
	bool const use_chi/*=false*/
) const
{
	using namespace basic;
	interpolated_rotamer.packed_rotno() = packed_rotno;
	utility::fixedsizearray1< Size, ( 1 << N ) > sorted_rotno;
	utility::fixedsizearray1< PackedDunbrackRotamer< T, N >, 1 << N > rot;
	utility::fixedsizearray1< utility::fixedsizearray1< Real, ( 1 << N ) >, ( 1 << N ) > n_derivs;
	utility::fixedsizearray1< Real, ( 1 << N ) > rotprob;

	Size const this_bb_index( make_index< N >( N_BB_BINS, bb_bin ) );

	for ( Size sri = 1; sri <= ( 1 << N ); ++sri ) {
		Size const index( make_conditional_index< N >( N_BB_BINS, sri, bb_bin_next, bb_bin ) );
		Size const packed_rotno_next( ( canonical_aa_ && canonicals_use_voronoi_ ) || ( !canonical_aa_ && noncanonicals_use_voronoi_ ) ? make_conditional_packed_rotno_index( this_bb_index, index, packed_rotno, chi, use_chi ) : packed_rotno );
		sorted_rotno[ sri ] = packed_rotno_2_sorted_rotno_( index, packed_rotno_next );
		// rot.push_back( rotamers_( index, sorted_rotno[ sri ] ) ); // RM - This may be a bug from SHA1 38188ffecf06, but ignoring this preserves behavior because this copy was "off the end" of anything actually used
		for ( Size i = 1; i <= T; ++i ) rot[ sri ].chi_mean( i ) = rotamers_( index, sorted_rotno[ sri ] ).chi_mean( i );
		for ( Size i = 1; i <= T; ++i ) rot[ sri ].chi_sd(   i ) = rotamers_( index, sorted_rotno[ sri ] ).chi_sd(   i );
		rot[ sri ].rotamer_probability() = rotamers_( index, sorted_rotno[ sri ] ).rotamer_probability();
		for ( Size di = 1; di <= ( 1 << N ); ++di ) {
			n_derivs[ di ][ sri ] = rotamers_( index, sorted_rotno[ sri ] ).n_derivs()[ di ];
		}
		rotprob[ sri ] = static_cast< Real >( rot[ sri ].rotamer_probability() );
		if ( rotprob[ sri ] <= 1e-6 ) rotprob[ sri ] = 1e-6;
	}

	utility::fixedsizearray1< Real, N > scratch_drotprob_dbb;

	/// Note: with bicubic splines to interpolate the energy for a rotamer, bilinear
	/// interpolation of the rotamer probabilities no longer serves a useful purpose
	/// for the 2002 library.  However, the 2008 and 2010 libraries are not yet up to
	/// speed with cubic interpolation (requiring tricubic splines), so keep this value
	/// around for the moment.  Also being preserved for the sake of backwards compatibility.
	{//scope guards to prevent the binw variable from leaking
		utility::fixedsizearray1< Real, N > binw( BB_BINRANGE );
		interpolate_polylinear_by_value( rotprob, bb_alpha, binw, false /*Do not treat as angles*/ ,
			scratch.rotprob(), scratch_drotprob_dbb );
	}
	//std::cout << "********** After interpolate_polylinear_by_value, scratch.rotprob()=" << scratch.rotprob() << std::endl;

	for ( Size i = 1; i <= N; ++i ) scratch.drotprob_dbb()[ i ] = scratch_drotprob_dbb[ i ];

	if ( use_bicubic() && !peptoid_ /*Polycubic interpolation has problems with peptoids, it seems.*/ ) {

		utility::fixedsizearray1< Real, N > binw( BB_BINRANGE );
		utility::fixedsizearray1< Real, N > scratch_dneglnrotprob_dbb;

		//DELETE THE FOLLOWING -- FOR TESTING ONLY:
		//for(core::Size i(1), imax(n_derivs.size()); i<=imax; ++i) {
		// std::cout << "*** n_derivs[" << i << "]=" << n_derivs[i] << std::endl;
		//}

		polycubic_interpolation( n_derivs, bb_alpha, binw, scratch.negln_rotprob(), scratch_dneglnrotprob_dbb );
		for ( Size i = 1; i <= N; ++i ) scratch.dneglnrotprob_dbb()[ i ] = scratch_dneglnrotprob_dbb[ i ];
		interpolated_rotamer.rotamer_probability() = std::exp( -scratch.negln_rotprob() );
		//std::cout << "********** After polycubic_interpolation, interpolated_rotamer.rotamer_probability()=" << interpolated_rotamer.rotamer_probability() << std::endl;

	} else {
		interpolated_rotamer.rotamer_probability() = scratch.rotprob();
		scratch.negln_rotprob() = -std::log( scratch.rotprob() );
	}

	// Entropy correction
	if ( dun_entropy_correction() ) {
		// populate S_n_derivs
		utility::fixedsizearray1< utility::fixedsizearray1< Real, ( 1 << N ) >, ( 1 << N ) > S_n_derivs;
		for ( Size i = 1; i <= ( 1 << N ); ++i ) {
			for ( Size j = 1; j <= ( 1 << N ); ++j ) S_n_derivs[i][j] = ShannonEntropy_n_derivs_[ i ][ j ];
		}
		utility::fixedsizearray1< Real, N > binw( BB_BINRANGE );

		utility::fixedsizearray1< Real, N > scratch_dentropy_dbb;
		polycubic_interpolation( S_n_derivs, bb_alpha, binw, scratch.entropy(), scratch_dentropy_dbb );
		for ( Size i = 1; i <= N; ++i ) scratch.dentropy_dbb()[ i ] = scratch_dentropy_dbb[ i ];
	}

	for ( Size ii = 1; ii <= T; ++ii ) {

		utility::fixedsizearray1< Real, ( 1 << N ) > chi_mean;
		utility::fixedsizearray1< Real, ( 1 << N ) > chi_sd;
		for ( Size roti = 1; roti <= ( 1 << N ); ++roti ) {
			chi_mean[ roti ] = static_cast< Real >( rot[ roti ].chi_mean( ii ) );
			chi_sd[ roti ]   = static_cast< Real >( rot[ roti ].chi_sd( ii ) );
		}

		utility::fixedsizearray1< Real, N > scratch_dchi_mean;
		utility::fixedsizearray1< Real, N > scratch_dchi_sd;

		for ( Size bbi = 1; bbi <= N; ++bbi ) {
			scratch_dchi_mean[ bbi ] = scratch.dchimean_dbb()[ bbi ][ ii ];
			scratch_dchi_sd[ bbi ]   = scratch.dchisd_dbb()[   bbi ][ ii ];
		}
		utility::fixedsizearray1< Real, N > binw( BB_BINRANGE );

		interpolate_polylinear_by_value( chi_mean, bb_alpha, binw, true, scratch.chimean()[ ii ], scratch_dchi_mean );
		interpolated_rotamer.chi_mean( ii ) = scratch.chimean()[ ii ];
		//std::cout << "********** After interpolate_polylinear_by_value, scratch.chimean()[" << ii << "]=" << scratch.chimean()[ii] << std::endl;
		interpolate_polylinear_by_value( chi_sd, bb_alpha, binw, false, scratch.chisd()[ ii ], scratch_dchi_sd );
		interpolated_rotamer.chi_sd( ii ) = scratch.chisd()[ ii ];
		//std::cout << "********** After interpolate_polylinear_by_value, scratch.chi_sd()[" << ii << "]=" << scratch.chisd()[ii] << std::endl;

		for ( Size bbi = 1; bbi <= N; ++bbi ) {
			scratch.dchimean_dbb()[ bbi ][ ii ] = scratch_dchi_mean[ bbi ];
			scratch.dchisd_dbb()[ bbi ][ ii ]   = scratch_dchi_sd[ bbi ];
		}
	}
}

/// @brief Given the index of a rotamer in the current backbone bin, find the closest rotamer index in another
/// backbone bin.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
template< Size T, Size N >
Size
RotamericSingleResidueDunbrackLibrary< T, N >::make_conditional_packed_rotno_index(
	Size const original_bb_index,
	Size const bb_index,
	Size const packed_rotno,
	ChiVector const & chi,
	bool const use_chi
) const {
	if ( original_bb_index == bb_index ) return packed_rotno;

	PackedDunbrackRotamer< T, N > const &cur_rot( rotamers_( original_bb_index, packed_rotno_2_sorted_rotno_( original_bb_index, packed_rotno ) ) );

	core::Real min_distsq(0);
	bool first(true);
	core::Size lowest_rotindex(0);
	for ( core::Size i(1), imax(rotamers_.size2()); i<=imax; ++i ) {
		PackedDunbrackRotamer< T, N > const &comparison_rot( rotamers_( bb_index, packed_rotno_2_sorted_rotno_( bb_index, i ) ) );
		core::Real distsq(0);
		for ( core::Size j(1); j <= T; ++j ) {
			if ( !use_chi ) {
				distsq += std::pow( basic::subtract_degree_angles(comparison_rot.chi_mean(j), cur_rot.chi_mean(j)), 2 );
			} else {
				distsq += std::pow( basic::subtract_degree_angles(comparison_rot.chi_mean(j), chi[j]), 2 );
			}
		}
		if ( first || distsq < min_distsq ) {
			first = false;
			min_distsq = distsq;
			lowest_rotindex = comparison_rot.packed_rotno();
		}
	}
	debug_assert(lowest_rotindex != 0);

	//std::cout << "****** original_bb_index=" << original_bb_index << "\tbb_index=" << bb_index << "\tpacked_rotno=" << packed_rotno << "\tlowest_rotindex=" << lowest_rotindex << std::endl;

	return lowest_rotindex;
}

/// @details Handle lower-term residues by returning a "neutral" phi value
template < Size T, Size N >
Real
RotamericSingleResidueDunbrackLibrary< T, N >::get_phi_from_rsd(
	conformation::Residue const & rsd
) const
{
	debug_assert( rsd.is_protein() || rsd.is_peptoid() );
	Real const d_multiplier = rsd.type().is_mirrored_type() ? -1.0 : 1.0;
	if ( rsd.is_lower_terminus() || !rsd.has_lower_connect() ) return d_multiplier * parent::PEPTIDE_NEUTRAL_PHI;
	else return rsd.mainchain_torsion( 1 );
}

/// @details Handle upper-term residues by returning a "neutral" psi value
template < Size T, Size N >
Real
RotamericSingleResidueDunbrackLibrary< T, N >::get_psi_from_rsd(
	conformation::Residue const & rsd
) const
{
	debug_assert( rsd.is_protein() || rsd.is_peptoid() );
	Real const d_multiplier = rsd.type().is_mirrored_type() ? -1.0 : 1.0;
	if ( rsd.is_upper_terminus() || !rsd.has_upper_connect() ) return d_multiplier * parent::PEPTIDE_NEUTRAL_PHI;
	else return rsd.mainchain_torsion( 2 );
}

/// @details Handle lower-term residues by returning a "neutral" phi value
template < Size T, Size N >
Real
RotamericSingleResidueDunbrackLibrary< T, N >::get_IV_from_rsd(
	Size bbn,
	conformation::Residue const & rsd,
	pose::Pose const & pose
) const
{
	return iv_reporters_[ bbn ]->get_dof( rsd, pose );
}

/// @details Handle upper-term residues by returning a "neutral" psi value
template < Size T, Size N >
utility::fixedsizearray1< Real, N >
RotamericSingleResidueDunbrackLibrary< T, N >::get_IVs_from_rsd(
	conformation::Residue const & rsd,
	pose::Pose const & pose
) const
{
	utility::fixedsizearray1< Real, N > tors;

	for ( Size ii = 1; ii <= N; ++ii ) {
		tors[ ii ] = iv_reporters_[ ii ]->get_dof( rsd, pose );
	}

	return tors;
}


template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::fill_rotamer_vector(
	pose::Pose const & pose,
	scoring::ScoreFunction const & scorefxn,
	pack::task::PackerTask const & task,
	utility::graph::GraphCOP packer_neighbor_graph,
	chemical::ResidueTypeCOP concrete_residue,
	conformation::Residue const & existing_residue,
	utility::vector1< utility::vector1< Real > > const & extra_chi_steps,
	bool buried,
	rotamers::RotamerVector & rotamers
) const
{
	///Determine whether this is a D-amino acid:
	const core::Real d_multiplier = existing_residue.type().is_mirrored_type() ? -1.0 : 1.0;

	/// Save backbone interpolation data for reuse
	utility::fixedsizearray1< Real, N > bbs = get_IVs_from_rsd( existing_residue, pose );
	for ( Size bbi = 1; bbi <= N; ++bbi ) bbs[ bbi ] *= d_multiplier;
	utility::fixedsizearray1< Size, N > bb_bin, bb_bin_next;
	utility::fixedsizearray1< Real, N > bb_alpha;
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

	Real const requisit_probability = probability_to_accumulate_while_building_rotamers( task, buried );
	Real accumulated_probability( 0.0 );

	Size const max_rots_that_can_be_built = n_packed_rots();
	Size count_rotamers_built = 0;
	RotamerLibraryInterpolationScratch scratch;
	while ( accumulated_probability < requisit_probability ) {
		// Iterate through rotamaers in decreasing order of probabilities, stopping once the requisit probility is hit.
		++count_rotamers_built;

		Size index = make_index< N >( N_BB_BINS, bb_bin );
		Size const packed_rotno00 = rotamers_( index, count_rotamers_built ).packed_rotno();
		PackedDunbrackRotamer< T, N, Real > interpolated_rotamer;
		interpolate_rotamers( scratch, packed_rotno00, bb_bin, bb_bin_next, bb_alpha, interpolated_rotamer );

		build_rotamers( pose, scorefxn, task, packer_neighbor_graph,
			concrete_residue, existing_residue, extra_chi_steps, buried, rotamers,
			interpolated_rotamer );

		accumulated_probability += interpolated_rotamer.rotamer_probability();
		if ( count_rotamers_built == max_rots_that_can_be_built ) break; // this shouldn't happen...
	}
}

template < Size T, Size N >
utility::vector1< DunbrackRotamerSampleData >
RotamericSingleResidueDunbrackLibrary< T, N >::get_all_rotamer_samples(
	Real5 bbs2
) const
{
	utility::fixedsizearray1< Size, N > bb_bin, bb_bin_next;
	utility::fixedsizearray1< Real, N > bb_alpha;

	utility::fixedsizearray1< Real, N > bbs;
	for ( Size i = 1 ; i <= N; ++i ) bbs[ i ] = bbs2[ i ];
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

	Size const n_rots = n_packed_rots();
	utility::vector1< DunbrackRotamerSampleData > all_rots;
	all_rots.reserve( n_rots );

	RotamerLibraryInterpolationScratch scratch;
	for ( Size ii = 1; ii <= n_rots; ++ii ) {
		// Iterate through rotamaers in decreasing order of probabilities
		Size index = make_index< N > ( N_BB_BINS, bb_bin );
		Size const packed_rotno00 = rotamers_( index, ii ).packed_rotno();
		PackedDunbrackRotamer< T, N, Real > interpolated_rotamer;
		interpolate_rotamers( scratch, packed_rotno00, bb_bin, bb_bin_next, bb_alpha, interpolated_rotamer );

		DunbrackRotamerSampleData sample( false );
		sample.set_nchi( T );
		sample.set_rotwell( parent::packed_rotno_2_rotwell( interpolated_rotamer.packed_rotno() ));
		for ( Size jj = 1; jj <= T; ++jj ) sample.set_chi_mean( jj, interpolated_rotamer.chi_mean( jj ) );
		for ( Size jj = 1; jj <= T; ++jj ) sample.set_chi_sd( jj, interpolated_rotamer.chi_sd( jj ) );
		sample.set_prob( interpolated_rotamer.rotamer_probability() );

		all_rots.push_back( sample );
	}
	return all_rots;
}

template < Size T, Size N >
Real
RotamericSingleResidueDunbrackLibrary< T, N >::get_probability_for_rotamer(
	utility::fixedsizearray1< Real, N > bbs,
	Size rot_ind
) const
{
	Size const n_rot = 1 << N;

	utility::fixedsizearray1< Size, N >  bb_bin, bb_bin_next;
	utility::fixedsizearray1< Real, N >  bb_alpha;
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

	utility::vector1< PackedDunbrackRotamer< T, N > > rot;
	utility::fixedsizearray1< Size, n_rot > sorted_rotno;

	Size ind00 = make_index< N >( N_BB_BINS, bb_bin );
	utility::fixedsizearray1< Real, n_rot > interp_probs;
	rot.push_back( rotamers_( ind00, rot_ind ) );
	rot[ 1 ].rotamer_probability() = rotamers_( ind00, rot_ind ).rotamer_probability();
	interp_probs[ 1 ] = static_cast< Real >( rot[ 1 ].rotamer_probability() );
	Size const packed_rotno00 = rot[ 1 ].packed_rotno();

	for ( Size indi = 2; indi <= n_rot; ++indi ) {
		Size index = make_conditional_index< N >( N_BB_BINS, indi, bb_bin_next, bb_bin );
		sorted_rotno[ indi ] = packed_rotno_2_sorted_rotno_( index, packed_rotno00 );
		rot.push_back( rotamers_( index, sorted_rotno[ indi ] ) );
		rot[ indi ].rotamer_probability() = rotamers_( index, sorted_rotno[ indi ] ).rotamer_probability();
		interp_probs[ indi ] = static_cast< Real >( rot[ indi ].rotamer_probability() );
	}

	Real rot_prob;
	utility::fixedsizearray1< Real, N > dummy;
	utility::fixedsizearray1< Real, N > binw( BB_BINRANGE );

	interpolate_polylinear_by_value( interp_probs, bb_alpha, binw, false, rot_prob, dummy );
	//std::cout << "********** After interpolate_polylinear_by_value, rot_prob=" << rot_prob << std::endl;
	return rot_prob;
}

template < Size T, Size N >
DunbrackRotamerSampleData
RotamericSingleResidueDunbrackLibrary< T, N >::get_rotamer(
	utility::fixedsizearray1< Real, N > bbs,
	Size rot_ind
) const
{
	utility::fixedsizearray1< Size, N > bb_bin, bb_bin_next;
	utility::fixedsizearray1< Real, N > bb_alpha;
	get_bb_bins( bbs, bb_bin, bb_bin_next, bb_alpha );

	Size index = make_index< N >( N_BB_BINS, bb_bin );
	PackedDunbrackRotamer< T, N > const & rot00( rotamers_( index, rot_ind ) );
	Size const packed_rotno00 = rot00.packed_rotno();
	PackedDunbrackRotamer< T, N, Real > interpolated_rotamer;
	RotamerLibraryInterpolationScratch scratch;
	interpolate_rotamers( scratch, packed_rotno00, bb_bin, bb_bin_next, bb_alpha, interpolated_rotamer );

	DunbrackRotamerSampleData sample( false );
	sample.set_nchi( T );
	sample.set_rotwell( parent::packed_rotno_2_rotwell( interpolated_rotamer.packed_rotno() ));
	for ( Size jj = 1; jj <= T; ++jj ) sample.set_chi_mean( jj, interpolated_rotamer.chi_mean( jj ) );
	for ( Size jj = 1; jj <= T; ++jj ) sample.set_chi_sd( jj, interpolated_rotamer.chi_sd( jj ) );
	sample.set_prob( interpolated_rotamer.rotamer_probability() );

	return sample;
}

template < Size T, Size N >
Size
RotamericSingleResidueDunbrackLibrary< T, N >::nchi() const
{
	return T;
}

template < Size T, Size N >
Size
RotamericSingleResidueDunbrackLibrary< T, N >::nbb() const
{
	return N;
}

template < Size T, Size N >
Size
RotamericSingleResidueDunbrackLibrary< T, N >::n_rotamer_bins() const
{
	return parent::n_possible_rots();
}


/// @details Load interpolated rotamer data into a RotamericData object on the stack
/// so that it can be handed into the enumerate_chi_sets method, which itself relies
/// on the "template method" chisamples_for_rotamer_and_chi.  Enumerate the chi samples,
/// and build rotamers from these chi samples.
///
/// "template method" is a design pattern where a base class calls a polymorphic method
/// that can be overloaded by a derived class, usually in the middle of a function that
/// does a lot of work.  See "Design Patterns," Gamma et al.
template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::build_rotamers(
	pose::Pose const & pose,
	scoring::ScoreFunction const & scorefxn,
	pack::task::PackerTask const & task,
	utility::graph::GraphCOP packer_neighbor_graph,
	chemical::ResidueTypeCOP concrete_residue,
	conformation::Residue const& existing_residue,
	utility::vector1< utility::vector1< Real > > const & extra_chi_steps,
	bool buried,
	rotamers::RotamerVector & rotamers,
	PackedDunbrackRotamer< T, N, Real > const & interpolated_rotamer
) const
{
	DunbrackRotamer< T, N, Real > interpolated_rot( packed_rotamer_2_regular_rotamer( interpolated_rotamer ) );
	interpolated_rot.rotamer_probability() = interpolated_rotamer.rotamer_probability();
	RotamericData< T, N > rotameric_rotamer_building_data( interpolated_rot );

	// now build the chi sets derived from this base rotamer
	utility::vector1< ChiSetOP > chi_set_vector;
	enumerate_chi_sets(
		*concrete_residue, task, existing_residue.seqpos(), buried,
		rotameric_rotamer_building_data,
		extra_chi_steps, chi_set_vector );

	create_rotamers_from_chisets(
		pose, scorefxn, task,
		packer_neighbor_graph, concrete_residue, existing_residue,
		chi_set_vector, rotamers );
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::verify_bb_bins(
	utility::fixedsizearray1< Real, N > const & bbs,
	utility::fixedsizearray1< Size, N > const & bb_bin,
	utility::fixedsizearray1< Size, N > const & bb_bin_next
) const
{
	for ( Size ii = 1; ii <= N; ++ii ) {
		if ( ( bb_bin[ ii ] < 1 || bb_bin[ ii ] > 36 ) || ( bb_bin_next[ ii ] < 1 || bb_bin_next[ ii ] > 36 ) ) {
			std::cerr << "ERROR: bb " << ii << " bin out of range: " <<
				aa() << " " << bbs[ ii ] << " " << bb_bin[ ii ] << " " << bb_bin_next[ ii ] <<  std::endl;
			utility_exit();
		}
	}
}

/// @details once a list of chi samples has been enumerated, this function
/// instantiates Residue objectes and give them the correct geometry.
template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::create_rotamers_from_chisets(
	pose::Pose const & pose,
	scoring::ScoreFunction const & scorefxn,
	pack::task::PackerTask const & task,
	utility::graph::GraphCOP packer_neighbor_graph,
	chemical::ResidueTypeCOP concrete_residue,
	conformation::Residue const& existing_residue,
	utility::vector1< ChiSetOP > const & chi_set_vector,
	rotamers::RotamerVector & rotamers
) const
{
	using namespace utility;
	pack::task::ResidueLevelTask const & rtask( task.residue_task( existing_residue.seqpos() ) );

	// construct real rotamers
	for ( vector1< ChiSetOP >::const_iterator chi_set( chi_set_vector.begin() );
			chi_set != chi_set_vector.end(); ++chi_set ) {

		conformation::ResidueOP rotamer = conformation::ResidueFactory::create_residue(
			*concrete_residue, existing_residue, pose.conformation(), rtask.preserve_c_beta() );
		for ( Size jj = 1; jj <= (*chi_set)->chi.size(); ++jj ) rotamer->set_chi( jj, (*chi_set)->chi[ jj ] );

		utility::vector1< conformation::ResidueOP > rotamer_conformers;
		rotamer_conformers.push_back( rotamer );

		// Possible ring conformers.
		if ( rotamer->type().is_cyclic() ) {
			for ( Size rn = 1; rn <= rotamer->type().n_rings(); ++rn ) {

				if ( ! rotamer->type().ring_conformer_set(rn)->low_energy_conformers_are_known() ) continue;

				Size first_nu_for_ring = 1, last_nu_for_ring = rotamer->type().ring_atoms( rn ).size() - 1;
				for ( Size rn2 = 1; rn2 < rn; ++rn2 ) {
					first_nu_for_ring += rotamer->type().ring_atoms( rn2 ).size() - 1;
					last_nu_for_ring += rotamer->type().ring_atoms( rn2 ).size() - 1;
				}

				// This conceit--accumulating a vector of conformers
				// and only evaluating once at the end--allows us to avoid the
				// janky unknown depth for loop across number of rings.
				for ( Size rc = 1; rc <= rotamer_conformers.size(); ++rc ) {
					Size ring_atoms_in_backbone = 0;
					for ( Size ri = 1; ri <= rotamer->type().ring_atoms( rn ).size(); ++ri ) {
						if ( rotamer->type().atom_is_backbone( rotamer->type().ring_atoms( rn )[ ri ] ) ) {
							++ring_atoms_in_backbone;
						}
					}
					if ( ring_atoms_in_backbone > 1 ) continue;

					utility::vector1< chemical::rings::RingConformer > confs = rotamer->type().ring_conformer_set( rn )->get_local_min_conformers();
					for ( Size i = 1; i <= confs.size(); ++i ) {

						conformation::ResidueOP conformer = utility::pointer::make_shared< conformation::Residue >( *rotamer );

						//conformer->set_all_nu( confs[ i ].nu_angles, confs[ i ].tau_angles );
						conformer->set_all_ring_nu( first_nu_for_ring, last_nu_for_ring, confs[ i ].nu_angles, confs[ i ].tau_angles );

						rotamer_conformers.push_back( conformer );
					}
				}
			}
		}
		for ( Size ak = 1; ak <= rotamer_conformers.size(); ++ak ) {
			// apply an operation (or a filter) to this rotamer at build time
			bool reject(false);
			for ( pack::rotamer_set::RotamerOperations::const_iterator
					op( rtask.rotamer_operations().begin() );
					op != rtask.rotamer_operations().end(); ++op ) {
				reject |= ! (**op)( rotamer_conformers[ak], pose, scorefxn, rtask, packer_neighbor_graph, *chi_set );
			}
			if ( !reject ) rotamers.push_back( rotamer_conformers[ak] );
		}
	}
}

template< Size T, Size N >
template< class P >
DunbrackRotamer< T, N, P >
RotamericSingleResidueDunbrackLibrary< T, N >::packed_rotamer_2_regular_rotamer(
	PackedDunbrackRotamer< T, N, P > const & packedrot
) const
{
	DunbrackRotamer< T, N, P > dunrot;
	for ( Size ii = 1; ii <= T; ++ii ) {
		dunrot.chi_mean( ii ) = packedrot.chi_mean( ii );
		dunrot.chi_sd(   ii ) = packedrot.chi_sd( ii );
		dunrot.rotwell(  ii ) = packed_rotno_2_rotwell( packedrot.packed_rotno() )[ ii ];
	}
	dunrot.rotamer_probability() = packedrot.rotamer_probability();
	return dunrot;
}


template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::enumerate_chi_sets(
	chemical::ResidueType const & rsd_type,
	pack::task::PackerTask const & task,
	Size const seqpos,
	bool buried,
	RotamericData< T, N > const & rotamer_data,
	utility::vector1< utility::vector1< Real > > const & extra_chi_steps,
	utility::vector1< ChiSetOP > & chi_set_vector
) const
{
	Real const base_probability( rotamer_data.rotamer().rotamer_probability() );
	using namespace utility;
	Size const nchi( rsd_type.nchi() );

	vector1< vector1< Real > > total_chi( nchi );
	vector1< vector1< int  > > total_rot( nchi );
	vector1< vector1< Real > > total_ex_steps( nchi ); // remember which extra chi steps were used for each rotamer
	vector1< vector1< Real > > chisample_prob( nchi ); // let derived class define sample probability

	pack::task::ResidueLevelTask const & rtask( task.residue_task( seqpos ) );
	/// List all chi samples for each chi
	for ( Size ii = 1; ii <= nchi; ++ii ) {
		chisamples_for_rotamer_and_chi(
			rsd_type, rtask, buried, ii, rotamer_data, extra_chi_steps[ ii ],
			total_chi[ ii ], total_rot[ ii ], total_ex_steps[ ii ], chisample_prob[ ii ]
		);
	}

	// now convert from total_chi to chisets....

	Size exchi_product = 1;
	utility::vector1< Size > lex_sizes( total_chi.size() );
	for ( Size ii = 1; ii <= nchi; ++ii ) {
		lex_sizes[ ii ] = total_chi[ ii ].size();
		exchi_product *= total_chi[ ii ].size();
	}
	chi_set_vector.reserve( exchi_product );

	// previously named min_extrachi_rot_prob in rosetta++
	Real const minimum_probability_for_extra_rotamers = 0.001; // make this a virtual function call...

	bool first_rotamer( true );
	for ( utility::LexicographicalIterator lex( lex_sizes ); ! lex.at_end(); ++lex ) {
		Real chi_set_prob = base_probability;

		runtime_assert( nchi <=  lex.size() );
		runtime_assert( nchi <=  chisample_prob.size() );

		for ( Size ii = 1; ii <= nchi; ++ii ) {
			if ( chisample_prob[ ii ].size() >= lex[ ii ] ) chi_set_prob *= chisample_prob[ ii ][ lex[ ii ] ];
		}

		if ( ! first_rotamer && chi_set_prob < minimum_probability_for_extra_rotamers ) continue;

		first_rotamer = false;

		ChiSetOP chi_set( new ChiSet( nchi ) );
		for ( Size ii = 1; ii <= nchi; ++ii ) {
			chi_set->chi[ ii ] = total_chi[ ii ][ lex[ ii ] ];
			chi_set->ex_chi_steps[ ii ] = total_ex_steps[ ii ][ lex[ ii ] ];
			chi_set->rot[ ii ] = total_rot[ ii ][ lex[ ii ] ];
		}
		chi_set_vector.push_back( chi_set );
	}
}


template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::chisamples_for_rotamer_and_chi(
	chemical::ResidueType const & rsd_type,
	pack::task::ResidueLevelTask const & rtask,
	bool buried,
	Size const chi_index,
	RotamericData< T, N > const & rotamer_data,
	utility::vector1< Real > const & extra_steps,
	utility::vector1< Real > & total_chi,
	utility::vector1< int  > & total_rot,
	utility::vector1< Real > & total_ex_steps,
	utility::vector1< Real > & chisample_prob
) const
{

	//debug_assert( dynamic_cast< RotamericData const & > ( rotamer_data ) );
	//RotamericData const & rotameric_data( static_cast< RotamericData const & > ( rotamer_data ) );

	// setting this value to zero disables the small-standdev filter -- is that what we want?
	DunbrackReal const min_extrachi_sd( 0.0 );

	if ( chi_index > T ) {
		// chi angle not present in dunbrack library, eg. ser,thr,tyr Hchi

		utility::vector1< std::pair< Real, Real > > const & chi_rotamers( rsd_type.chi_rotamers( chi_index ) );
		if ( ! chi_rotamers.empty() ) {
			// use the rotamer info encoded in the residue
			for ( Size j = 1; j <= chi_rotamers.size(); ++j ) {
				Real const rot_chi_mean( chi_rotamers[ j ].first );
				Real const rot_chi_sdev( chi_rotamers[ j ].second );
				total_chi.push_back( rot_chi_mean );
				total_ex_steps.push_back( 0. );
				total_rot.push_back( j );
				for ( Size k = 1; k <= extra_steps.size(); ++k ) {
					total_chi.push_back( rot_chi_mean + extra_steps[ k ] * rot_chi_sdev );
					total_ex_steps.push_back( extra_steps[ k ] );
					total_rot.push_back( j );
					chisample_prob.push_back( 1.0 ); // assume perfectly likely rotamer sample?
				}
			}
		} else if ( rsd_type.is_proton_chi( chi_index ) ) {
			pack::task::ExtraRotSample ex_samp_level = rtask.extrachi_sample_level( buried, chi_index, rsd_type );
			bool const skip_extra_proton_chi_samples = ( ex_samp_level == pack::task::NO_EXTRA_CHI_SAMPLES );

			Size const proton_chi_index = rsd_type.chi_2_proton_chi( chi_index );
			utility::vector1< Real > const & samples = rsd_type.proton_chi_samples( proton_chi_index );
			utility::vector1< Real > const & extra_samples = rsd_type.proton_chi_extra_samples( proton_chi_index );
			for ( Size ii = 1; ii <= samples.size(); ++ii ) {
				total_chi.push_back( samples[ ii ] );
				total_ex_steps.push_back( 0.0 );
				total_rot.push_back( 1 );
				chisample_prob.push_back( 1.0 );

				if ( skip_extra_proton_chi_samples ) continue;

				for ( Size jj = 1; jj <= extra_samples.size(); ++jj ) {
					total_chi.push_back( samples[ ii ] + extra_samples[ jj ] );
					total_ex_steps.push_back( 0.0 );
					total_rot.push_back( 1 );
					chisample_prob.push_back( 1.0 );

					total_chi.push_back( samples[ ii ] - extra_samples[ jj ] );
					total_ex_steps.push_back( 0.0 );
					total_rot.push_back( 1 );
					chisample_prob.push_back( 1.0 );
				}
			}

		} else {
			// just use the chi of the platonic residue -- this could be made faster, but chi is not part of rsdtype intrfc
			// Is this code ever executed?  It's not going to be executed for any amino acid...
			Real const icoor_chi( numeric::dihedral(
				rsd_type.ideal_xyz( rsd_type.chi_atoms( chi_index )[ 1 ] ),
				rsd_type.ideal_xyz( rsd_type.chi_atoms( chi_index )[ 2 ] ),
				rsd_type.ideal_xyz( rsd_type.chi_atoms( chi_index )[ 3 ] ),
				rsd_type.ideal_xyz( rsd_type.chi_atoms( chi_index )[ 4 ] )
				));
			total_chi.push_back( icoor_chi );
			total_ex_steps.push_back( 0. );
			total_rot.push_back( 1 );
			chisample_prob.push_back( 1.0 );
		}
	} else {
		// use the dunbrack data
		total_chi.push_back( rotamer_data.rotamer().chi_mean( chi_index ) );
		total_ex_steps.push_back( 0. );
		total_rot.push_back( rotamer_data.rotamer().rotwell( chi_index ) );
		chisample_prob.push_back( 1.0 );

		// ctsa - eliminate extra chi angles with insignificant sd's
		if ( rotamer_data.rotamer().chi_sd( chi_index ) <= min_extrachi_sd ) return;

		for ( Size k = 1; k <= extra_steps.size(); ++k ) {
			total_chi.push_back( rotamer_data.rotamer().chi_mean( chi_index )
				+ extra_steps[k] * rotamer_data.rotamer().chi_sd( chi_index ) );
			total_ex_steps.push_back( extra_steps[k] );
			total_rot.push_back( rotamer_data.rotamer().rotwell( chi_index ) );
			/// prob = exp( -( x - xmean )**2 / 2sd**2). Since we're sampling at intrvals of sd,
			/// prob = exp( -( step * sd )**2 / 2sd**2 ) = exp( - step**2/2 ).
			/// of course, a true gaussian probability would be scaled by 1/sd(2p)**0.5...
			chisample_prob.push_back( std::exp( -0.5 * (extra_steps[ k ]*extra_steps[ k ])) );
		}
	}
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::write_to_file( utility::io::ozstream & /*out*/ ) const
{
	utility_exit_with_message("Unimplemented!");
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::write_to_binary( utility::io::ozstream & out ) const
{
	parent::write_to_binary( out );

	/// 1. rotamers_
	{
		Size const ntotalrot = product( N_BB_BINS ) * parent::n_packed_rots();
		Size const ntotalchi = ntotalrot * T;
		///a. means
		DunbrackReal * rotamer_means = new DunbrackReal[ ntotalchi ];
		// b. standard deviations
		DunbrackReal * rotamer_stdvs = new DunbrackReal[ ntotalchi ];
		// c. rotamer probabilities
		DunbrackReal * rotamer_probs = new DunbrackReal[ ntotalrot ];
		// d. bicubic polynomial parameters
		utility::vector1< core::Real * > rotamer_negln_n_derivs( 1 << N );
		for ( Size initi = 1; initi <= ( 1 << N ); ++initi ) {
			rotamer_negln_n_derivs[ initi ] = new core::Real[ ntotalrot ];
		}

		// e. packed rotamer numbers
		DunbrackReal * packed_rotnos = new DunbrackReal[ ntotalrot ];

		Size count_chi( 0 ), count_rots( 0 );
		for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {
			utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
			utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
			for ( Size i = 1; i <= N; ++i ) bb_bin_maxes = N_BB_BINS[i];
			Size p = 1;
			while ( bb_bin[ N + 1 ] == 1 ) {

				Size bb_rot_index = make_index< N >( N_BB_BINS, bb_bin );

				for ( Size ll = 1; ll <= T; ++ll ) {
					rotamer_means[ count_chi ] = rotamers_( bb_rot_index, ii ).chi_mean( ll );
					rotamer_stdvs[ count_chi ] = rotamers_( bb_rot_index, ii ).chi_sd( ll );
					++count_chi;
				}
				rotamer_probs[ count_rots ] = rotamers_( bb_rot_index, ii ).rotamer_probability();
				for ( Size deriv_i = 1; deriv_i <= ( 1 << N ); ++deriv_i ) {
					rotamer_negln_n_derivs[ deriv_i ][ count_rots ] = rotamers_( bb_rot_index, ii ).n_derivs()[ deriv_i ];
				}

				packed_rotnos[ count_rots ] = rotamers_( bb_rot_index, ii ).packed_rotno();
				++count_rots;

				bb_bin[ 1 ]++;
				while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
					bb_bin[ p ] = 1;
					if ( p < bb_bin.size() ) {
						bb_bin[ ++p ]++;
						if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
					} else {
						break;
					}
				}
			}
		}
		out.write( (char*) rotamer_means, ntotalchi * sizeof( DunbrackReal ));
		out.write( (char*) rotamer_stdvs, ntotalchi * sizeof( DunbrackReal ));
		out.write( (char*) rotamer_probs, ntotalrot * sizeof( DunbrackReal ));
		for ( Size writei = 1; writei <= ( 1 << N ); ++writei ) {
			out.write( (char*) rotamer_negln_n_derivs[ writei ], ntotalrot * sizeof( core::Real ));
		}

		out.write( (char*) packed_rotnos, ntotalrot * sizeof( DunbrackReal ));
		delete [] rotamer_means;
		delete [] rotamer_stdvs;
		delete [] rotamer_probs;
		for ( Size writei = 1; writei <= ( 1 << N ); ++writei ) {
			delete[] rotamer_negln_n_derivs[ writei ];
		}
		delete [] packed_rotnos;
	}

	/// 2. packed_rotno_to_sorted_rotno_
	{
		Size const ntotalpackedrots = product( N_BB_BINS ) * parent::n_packed_rots();
		boost::int32_t * packed_rotno_2_sorted_rotno = new boost::int32_t[ ntotalpackedrots ];
		Size count( 0 );
		for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {
			utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
			utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
			for ( Size i = 1; i <= N; ++i ) bb_bin_maxes = N_BB_BINS[i];
			Size p = 1;
			while ( bb_bin[ N + 1 ] == 1 ) {

				packed_rotno_2_sorted_rotno[ count ] = packed_rotno_2_sorted_rotno_( make_index< N >( N_BB_BINS, bb_bin ), ii );
				++count;

				bb_bin[ 1 ]++;
				while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
					bb_bin[ p ] = 1;
					bb_bin[ ++p ]++;
					if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
				}
			}
		}
		out.write( (char*) packed_rotno_2_sorted_rotno, ntotalpackedrots * sizeof( boost::int32_t ) );
		delete [] packed_rotno_2_sorted_rotno;
	}
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::read_from_binary( utility::io::izstream & in )
{
	parent::read_from_binary( in );
	Size n_rot_bins = product( N_BB_BINS );
	/// 1. rotamers_
	{
		Size const ntotalrot = n_rot_bins * parent::n_packed_rots();
		Size const ntotalchi = ntotalrot * T;
		rotamers_.dimension( n_rot_bins, parent::n_packed_rots(), PackedDunbrackRotamer< T, N >() );

		///a. means
		DunbrackReal * rotamer_means = new DunbrackReal[ ntotalchi ];
		// b. standard deviations
		DunbrackReal * rotamer_stdvs = new DunbrackReal[ ntotalchi ];
		// c. rotamer probabilities
		DunbrackReal * rotamer_probs = new DunbrackReal[ ntotalrot ];
		// d. bicubic polynomial parameters
		utility::vector1< core::Real * > rotamer_negln_n_derivs( 1 << N );

		in.read( (char*) rotamer_means, ntotalchi * sizeof( DunbrackReal ));
		in.read( (char*) rotamer_stdvs, ntotalchi * sizeof( DunbrackReal ));
		in.read( (char*) rotamer_probs, ntotalrot * sizeof( DunbrackReal ));
		for ( Size initi = 1; initi <= ( 1 << N ); ++initi ) {
			rotamer_negln_n_derivs[ initi ] = new core::Real[ ntotalrot ];
			in.read( (char*) rotamer_negln_n_derivs[ initi ], ntotalrot * sizeof( core::Real ));
		}
		// e. packed rotamer numbers
		DunbrackReal * packed_rotnos = new DunbrackReal[ ntotalrot ];

		in.read( (char*) packed_rotnos, ntotalrot * sizeof( DunbrackReal ));

		Size count_chi( 0 ), count_rots( 0 );
		for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {

			utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
			utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
			for ( Size i = 1; i <= N; ++i ) bb_bin_maxes = N_BB_BINS[i];
			Size p = 1;
			while ( bb_bin[ N + 1 ] == 1 ) {

				Size bb_rot_index = make_index< N >( N_BB_BINS, bb_bin );
				for ( Size ll = 1; ll <= T; ++ll ) {
					rotamers_( bb_rot_index, ii ).chi_mean( ll ) = rotamer_means[ count_chi ];
					rotamers_( bb_rot_index, ii ).chi_sd(  ll ) = rotamer_stdvs[ count_chi ];
					++count_chi;
				}
				rotamers_( bb_rot_index, ii ).rotamer_probability() = rotamer_probs[ count_rots ];
				for ( Size deriv_i = 1; deriv_i <= ( 1 << N ); ++deriv_i ) {
					rotamers_( bb_rot_index, ii ).n_derivs()[ deriv_i ] = rotamer_negln_n_derivs[ deriv_i ][ count_rots ];
				}
				rotamers_( bb_rot_index, ii ).packed_rotno( ( Size ) packed_rotnos[ count_rots ] );

				++count_rots;

				bb_bin[ 1 ]++;
				while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
					bb_bin[ p ] = 1;
					bb_bin[ ++p ]++;
					if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
				}
			}
		}

		delete [] rotamer_means;
		delete [] rotamer_stdvs;
		delete [] rotamer_probs;
		for ( Size deli = 1; deli <= ( 1 << N ); ++deli ) {
			delete [] rotamer_negln_n_derivs[ deli ];
		}
		delete [] packed_rotnos;
	}

	/// 2. packed_rotno_to_sorted_rotno_
	{
		Size const ntotalpackedrots = n_rot_bins * parent::n_packed_rots();
		boost::int32_t * packed_rotno_2_sorted_rotno = new boost::int32_t[ ntotalpackedrots ];
		in.read( (char*) packed_rotno_2_sorted_rotno, ntotalpackedrots * sizeof( boost::int32_t ) );
		Size count( 0 );
		packed_rotno_2_sorted_rotno_.dimension( n_rot_bins, parent::n_packed_rots() );
		for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {

			utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
			utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
			for ( Size i = 1; i <= N; ++i ) bb_bin_maxes = N_BB_BINS[i];
			Size p = 1;
			while ( bb_bin[ N + 1 ] == 1 ) {

				packed_rotno_2_sorted_rotno_( make_index< N >( N_BB_BINS, bb_bin ), ii ) = packed_rotno_2_sorted_rotno[ count ];
				++count;

				bb_bin[ 1 ]++;
				while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
					bb_bin[ p ] = 1;
					bb_bin[ ++p ]++;
					if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
				}
			}
		}
		delete [] packed_rotno_2_sorted_rotno;
	}

	/// Entropy setup once reading is finished
	if ( dun_entropy_correction() ) {
		setup_entropy_correction();
	}
}

/// @brief Comparison operator, mainly intended to use in ASCII/binary comparsion tests
/// Values tested should parallel those used in the read_from_binary() function.
template < Size T, Size N >
bool
RotamericSingleResidueDunbrackLibrary< T, N >::operator ==( SingleResidueRotamerLibrary const & rhs) const {
	static basic::Tracer TR( "core.pack.dunbrack.RotamericSingleResidueDunbrackLibrary" );

	// Raw pointer okay, we're just using it to check for conversion
	RotamericSingleResidueDunbrackLibrary< T, N > const * ptr( dynamic_cast< RotamericSingleResidueDunbrackLibrary< T, N > const * > ( &rhs ) );
	if ( ptr == nullptr ) {
		TR << "In comparison operator: right-hand side is not a matching RotamericSingleResidueDunbrackLibrary." << std::endl;
		return false;
	}
	RotamericSingleResidueDunbrackLibrary< T, N > const & other( dynamic_cast< RotamericSingleResidueDunbrackLibrary< T, N > const & > ( rhs ) );

	bool equal( true );

	if ( ! parent::operator==( rhs ) ) {
		//TR.Debug << "In RotamericSingleResidueDunbrackLibrary< T >::operator== : Parent comparsion returns false, stopping equality test." << std::endl;
		//return false;
		equal = false;
	}


	/// 1. rotamers_
	if (  parent::n_packed_rots() != other.n_packed_rots() ) {
		return false;
	}
	debug_assert( parent::n_packed_rots() == other.n_packed_rots() );
	for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {
		utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
		utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
		for ( Size i = 1; i <= N; ++i ) bb_bin_maxes = N_BB_BINS[i];
		Size p = 1;
		while ( bb_bin[ N + 1 ] == 1 ) {
			Size bb_rot_index = make_index< N >( N_BB_BINS, bb_bin );

			PackedDunbrackRotamer< T, N > const & this_rot( rotamers_( bb_rot_index, ii ) );
			PackedDunbrackRotamer< T, N > const & other_rot( other.rotamers_( bb_rot_index, ii ) );
			for ( Size ll = 1; ll <= T; ++ll ) {
				if ( ! numeric::equal_by_epsilon( this_rot.chi_mean( ll ), other_rot.chi_mean( ll ), ANGLE_DELTA ) ) {
					TR.Debug << "Comparsion failure in " << core::chemical::name_from_aa( aa() )
						<< " rotamers chi mean " << bb_bin[1] << " " << bb_bin[2] << " " << ii << " " << ll << " - "
						<< this_rot.chi_mean( ll ) << " vs. " <<  other_rot.chi_mean( ll ) << std::endl;
					equal = false;
				}
				if ( ! numeric::equal_by_epsilon( this_rot.chi_sd( ll ), other_rot.chi_sd( ll ), ANGLE_DELTA ) ) {
					TR.Debug << "Comparsion failure in " << core::chemical::name_from_aa( aa() )
						<< " rotamers chi sd " << bb_bin[1] << " " << bb_bin[2] << " " << ii << " " << ll << " - "
						<< this_rot.chi_sd( ll ) << " vs. " <<  other_rot.chi_sd( ll ) << std::endl;
					equal = false;
				}
			}
			if ( ! numeric::equal_by_epsilon( this_rot.rotamer_probability(), other_rot.rotamer_probability(), PROB_DELTA ) ) {
				TR.Debug << "Comparsion failure in " << core::chemical::name_from_aa( aa() )
					<< " rotamer probability " << bb_bin[1] << " " << bb_bin[2] << " " << ii << " - "
					<< this_rot.rotamer_probability() << " vs. " <<  other_rot.rotamer_probability() << std::endl;
				equal = false;
			}
			for ( Size n_deriv_index = 1; n_deriv_index <= (1 << N); ++n_deriv_index ) {
				if ( ! numeric::equal_by_epsilon( this_rot.n_derivs()[n_deriv_index], other_rot.n_derivs()[n_deriv_index], ENERGY_DELTA ) ) {
					TR.Debug << "Comparsion failure in " << core::chemical::name_from_aa( aa() )
						<< " deriv " << n_deriv_index << " " << bb_bin[1] << " " << bb_bin[2] << " " << ii << " - "
						<< this_rot.n_derivs()[n_deriv_index] << " vs. " <<  other_rot.n_derivs()[n_deriv_index] << std::endl;
					equal = false;
				}
			}
			if ( this_rot.packed_rotno() != other_rot.packed_rotno() ) {
				TR.Debug << "Comparsion failure in " << core::chemical::name_from_aa( aa() )
					<< " packed rotno " << bb_bin[1] << " " << bb_bin[2] << " " << ii << " - "
					<< this_rot.packed_rotno() << " vs. " <<  other_rot.packed_rotno() << std::endl;
				equal = false;
			}

			bb_bin[ 1 ]++;
			while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
				bb_bin[ p ] = 1;
				bb_bin[ ++p ]++;
				if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
			}
		}
	}

	/// 2. packed_rotno_to_sorted_rotno_
	for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {
		utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
		utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
		for ( Size i = 1; i <= N; ++i ) bb_bin_maxes = N_BB_BINS[i];
		Size p = 1;
		while ( bb_bin[ N + 1 ] == 1 ) {
			Size bb_rot_index = make_index< N >( N_BB_BINS, bb_bin );
			if ( packed_rotno_2_sorted_rotno_( bb_rot_index, ii ) != packed_rotno_2_sorted_rotno_( bb_rot_index, ii ) ) {
				TR.Debug << "Comparsion failure in " << core::chemical::name_from_aa( aa() )
					<< " packed_rotno_2_sorted_rotno " << bb_bin[1] << " " << bb_bin[2] << " " << ii << " - "
					<< packed_rotno_2_sorted_rotno_( bb_rot_index, ii ) << " vs. " << other.packed_rotno_2_sorted_rotno_( bb_rot_index, ii ) << std::endl;
				equal = false;
			}

			bb_bin[ 1 ]++;
			while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
				bb_bin[ p ] = 1;
				bb_bin[ ++p ]++;
				if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
			}
		}
	}

	/// Other variables ( ShannonEntropy_, S_dsecophi_, S_dsecopsi_, S_dsecophipsi_) are set up from the loaded data
	/// automatically - they should be equivalent if all other data is equivalent.

	return equal;
}

// Entropy
template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::setup_entropy_correction()
{
	// Iter again in order to reference ShannonEntropy
	for ( Size dimi = 1; dimi <= ( 1 << N ); ++dimi ) {
		ShannonEntropy_n_derivs_[ dimi ].dimension( product( N_BB_BINS ) );
	}

	// Get entropy values first
	utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
	utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
	for ( Size i = 1; i <= N; ++i ) bb_bin_maxes = N_BB_BINS[i];
	Size p = 1;
	while ( bb_bin[ N + 1 ] == 1 ) {

		Size bb_rot_index = make_index< N >( N_BB_BINS, bb_bin );

		// Initialize
		for ( Size deriv_i = 1; deriv_i <= ( 1 << N ); ++deriv_i ) {
			ShannonEntropy_n_derivs_[ deriv_i ]( bb_rot_index ) = 0.0;
		}

		// Divide probability by psum in order to make probability normalized
		// especially for semi-rotameric amino acids
		Real psum( 0.0 );
		for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {
			psum += rotamers_( bb_rot_index, ii ).rotamer_probability();
		}

		// The values actually stored are positive sign ( == negative entropy )
		// which corresponds to Free energy contribution by Entropy
		for ( Size ii = 1; ii <= parent::n_packed_rots(); ++ii ) {
			ShannonEntropy_n_derivs_[ 1 ]( bb_rot_index ) += -rotamers_( bb_rot_index, ii ).n_derivs()[ 1 ] * rotamers_( bb_rot_index, ii ).rotamer_probability() / psum;
		}

		bb_bin[ 1 ]++;
		while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
			bb_bin[ p ] = 1;
			bb_bin[ ++p ]++;
			if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
		}
	}

	/*
	std::cout << std::setw(3) << "AA";
	std::cout << " " << std::setw(6) << "Phi" << " " << std::setw(6) << "Psi";
	std::cout << " " << std::setw(8) << "Prot1" << " " << std::setw(8) << "Prot2" << " " << std::setw(8) << "Prot3";
	std::cout << " " << std::setw(8) << "S" << " " << std::setw(8) << "d2Sdphi2";
	std::cout << " " << std::setw(8) << "d2Sdpsi2" << " " << std::setw(8) << "d4Sdp2p2";
	std::cout << std::endl;
	*/

	// Derivatives

	for ( Size i = 1; i <= N+1; ++i ) bb_bin[ i ] = 1;
	p = 1;
	while ( bb_bin[ N + 1 ] == 1 ) {

		// AMW
		// Entropy derivative setup for each bb bin (used to be over jj, kk
		// phi/psi indices, but now you construct bb_bin from one plus the
		// first N indices and call make_index)

		// Set up denominators for each bb
		utility::fixedsizearray1< Real, N > dbb2( 0 );
		for ( Size ii = 1; ii <= N; ++ii ) {
			dbb2[ ii ] = 4.0 * BB_BINRANGE[ ii ] * BB_BINRANGE[ ii ];
		}

		// Make "this index"
		Size bin_index = make_index< N >( N_BB_BINS, bb_bin );

		// Make an array of "next indices" for each BB
		utility::fixedsizearray1< Real, N > next_bb_index( 0 );
		for ( Size ii = 1; ii <= N; ++ii ) {
			Size old_number = bb_bin[ ii ];
			if ( bb_bin[ ii ] == N_BB_BINS[ ii ] ) {
				bb_bin[ ii ] = 1;
			} else {
				++bb_bin[ ii ];
			}
			next_bb_index[ ii ] = make_index< N >( N_BB_BINS, bb_bin );
			bb_bin[ ii ] = old_number;
		}

		// Make an array of "previous indices" for each BB
		utility::fixedsizearray1< Real, N > prev_bb_index( 0 );
		for ( Size ii = 1; ii <= N; ++ii ) {
			Size old_number = bb_bin[ ii ];
			if ( bb_bin[ ii ] == 1 ) {
				bb_bin[ ii ] = N_BB_BINS[ ii ];
			} else {
				--bb_bin[ ii ];
			}
			prev_bb_index[ ii ] = make_index< N >( N_BB_BINS, bb_bin );
			bb_bin[ ii ] = old_number;
		}

		// Indices all acquired... now to calculate.
		// Note that ShannonEntropy_n_derivs_ has already been set for 1
		// because that is the value
		for ( Size deriv_i = 2; deriv_i <= ( 1 << N ); ++deriv_i ) {
			ShannonEntropy_n_derivs_[ deriv_i ]( bin_index ) = 1;
		}

		for ( Size deriv_i = 2; deriv_i <= ( 1 << N ); ++deriv_i ) {
			// No clever pre-computation available
			// (In the 2D case, you just wrote down dsecophi and dsecopsi
			// and dsecophipsi was the product)
			// No ordering guarantee in 3+ dimensions

			for ( Size bbi = 1; bbi <= N; ++bbi ) {
				// You get a factor of (next_val - prev_val) / dbb2[bbi] if
				// the derivative bit is set for this bb angle
				// Otherwise, factor of 1
				if ( ( deriv_i - 1 ) & ( 1 << ( N - bbi ) ) ) {
					ShannonEntropy_n_derivs_[ deriv_i ]( bin_index ) *=
						( ShannonEntropy_n_derivs_[ 1 ]( next_bb_index[ bbi ] )
						- ShannonEntropy_n_derivs_[ 1 ]( prev_bb_index[ bbi ] ) )
						/ dbb2[ bbi ];
				}
			}
		}

		bb_bin[ 1 ]++;
		while ( bb_bin[ p ] == bb_bin_maxes[p]+1 ) {
			bb_bin[ p ] = 1;
			bb_bin[ ++p ]++;
			if ( bb_bin[ p ] != bb_bin_maxes[p]+1 ) p = 1;
		}
	}
}

/// @brief Read from input stream; stream may contain data for other amino acids.
/// Quit once another amino acid is specified in the input file, returning the
/// name of the next amino acid specifed (since it's already been extracted from
/// the input stream).  Return the empty string if no other amino acid is specified.
/// @details Returns the three letter string of the next amino acid specified in the
/// input library.
/// @author Rewritten on 8 January 2018 by Vikram K. Mulligan (vmullig@uw.edu).
template < Size T, Size N >
std::string
RotamericSingleResidueDunbrackLibrary< T, N >::read_from_file(
	utility::io::izstream & infile,
	bool first_line_three_letter_code_already_read,
	bool write_out
)
{
	// Create the parser for the file.  The parser is a convenient place to put the parse functions, and a convenient place to store temporary
	// data during the parse:
	RotamericSingleResidueDunbrackLibraryParser parser( N, DUNBRACK_FILE_DEFAULT_SCTOR, n_possible_rots(), correct_rotamer_well_order_on_read_, peptoid_ && peptoid_is_achiral_, write_out );
	// Do the parse and analysis, and return the name of the next amino acid (or an empty string if there is no next amino acid):
	std::string const next_name( parser.read_file( infile, first_line_three_letter_code_already_read, aa() ) );
	parser.configure_rotameric_single_residue_dunbrack_library(*this, N_BB_BINS);

	initialize_bicubic_splines();
	/// Entropy setup once reading is finished
	if ( dun_entropy_correction() ) {
		setup_entropy_correction();
	}

	return next_name; // if we arived here, we got to the end of the file, so return the empty string.
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::initialize_bicubic_splines()
{
	// AMW: we have to treat the n_bb = 2 base case differently
	// because of how differently BicubicSpline works as an object
	// i.e. higher order splines solve problems by reducing to BicubicSpline as a base case
	// there may be a more elegant method to integrate the two but I don't know it.
	if ( N == 2 ) {
		for ( Size ii = 1; ii <= n_packed_rots(); ++ii ) {
			using namespace numeric;
			using namespace numeric::interpolation::spline;

			BicubicSpline rotEspline;
			MathMatrix< Real > energy_vals( N_BB_BINS[1], N_BB_BINS[2] );
			for ( Size jj = 0; jj < N_BB_BINS[1]; ++jj ) {
				for ( Size kk = 0; kk < N_BB_BINS[2]; ++kk ) {
					Size jp1kp1 = jj * N_BB_BINS[2] + kk+1;
					Size sortedrotno = packed_rotno_2_sorted_rotno_( jp1kp1, ii );
					PackedDunbrackRotamer< T, N > & rot = rotamers_( jp1kp1, sortedrotno );
					core::Real rotamer_energy = -std::log( core::Real(rot.rotamer_probability()) );
					rot.n_derivs()[ 1 ] = rotamer_energy;
					energy_vals( jj, kk ) = rotamer_energy;
				}
			}
			BorderFlag periodic_boundary[2] = { e_Periodic, e_Periodic };
			Real start_vals[2] = {0.0, 0.0}; // grid is placed on the ten-degree marks
			Real deltas[2] = {10.0, 10.0}; // grid is 10 degrees wide
			bool lincont[2] = {false,false}; //meaningless argument for a bicubic spline with periodic boundary conditions
			std::pair< Real, Real > unused[2];
			unused[0] = std::make_pair( 0.0, 0.0 );
			unused[1] = std::make_pair( 0.0, 0.0 );
			rotEspline.train( periodic_boundary, start_vals, deltas, energy_vals, lincont, unused );
			for ( Size jj = 0; jj < N_BB_BINS[1]; ++jj ) {
				for ( Size kk = 0; kk < N_BB_BINS[2]; ++kk ) {
					Size jp1kp1 = jj * N_BB_BINS[2] + kk+1;
					Size sortedrotno = packed_rotno_2_sorted_rotno_( jp1kp1, ii );
					PackedDunbrackRotamer< T, N > & rot = rotamers_( jp1kp1, sortedrotno );
					rot.n_derivs()[ 3 ] = rotEspline.get_dsecox()(  jj, kk );
					rot.n_derivs()[ 2 ] = rotEspline.get_dsecoy()(  jj, kk );
					rot.n_derivs()[ 4 ] = rotEspline.get_dsecoxy()( jj, kk );
				}
			}
		}
		// might as well hard-code case 3 to use tricubic splines
	} else if ( N == 3 ) {
		for ( Size ii = 1; ii <= n_packed_rots(); ++ii ) {
			using namespace numeric;
			using namespace numeric::interpolation::spline;

			TricubicSpline rotEspline;
			MathTensor< Real > energy_vals( N_BB_BINS[1], N_BB_BINS[2], N_BB_BINS[3] );
			for ( Size jj = 0; jj < N_BB_BINS[1]; ++jj ) {
				for ( Size kk = 0; kk < N_BB_BINS[2]; ++kk ) {
					for ( Size ll = 0; ll < N_BB_BINS[3]; ++ll ) {
						Size jp1kp1lp1 = jj * N_BB_BINS[3] * N_BB_BINS[2] + kk * N_BB_BINS[3] + ll+1;
						Size sortedrotno = packed_rotno_2_sorted_rotno_( jp1kp1lp1, ii );
						PackedDunbrackRotamer< T, N > & rot = rotamers_( jp1kp1lp1, sortedrotno );
						//rot.rotamer_probability() = rotamers_( jp1kp1lp1, sortedrotno ).rotamer_probability();
						//if ( rot.rotamer_probability() <= 1e-6 ) rot.rotamer_probability() = 1e-6;
						core::Real rotamer_energy = -std::log( core::Real(rot.rotamer_probability()) );
						rot.n_derivs()[ 1 ] = rotamer_energy;
						energy_vals( jj, kk, ll ) = rotamer_energy;
					}
				}
			}
			BorderFlag periodic_boundary[3] = { e_Periodic, e_Periodic, e_Periodic };
			Real start_vals[3] = {0.0, 0.0, 0.0}; // grid is placed on the ten-degree marks
			Real deltas[3] = {BB_BINRANGE[1], BB_BINRANGE[2], BB_BINRANGE[3]}; // grid is 10 degrees wide
			bool lincont[3] = {false,false,false}; //meaningless argument for a bicubic spline with periodic boundary conditions
			std::pair< Real, Real > unused[3];
			unused[0] = std::make_pair( 0.0, 0.0 );
			unused[1] = std::make_pair( 0.0, 0.0 );
			unused[2] = std::make_pair( 0.0, 0.0 );
			rotEspline.train( periodic_boundary, start_vals, deltas, energy_vals, lincont, unused );
			for ( Size jj = 0; jj < N_BB_BINS[1]; ++jj ) {
				for ( Size kk = 0; kk < N_BB_BINS[2]; ++kk ) {
					for ( Size ll = 0; ll < N_BB_BINS[3]; ++ll ) {
						Size jp1kp1lp1 = jj * N_BB_BINS[3] * N_BB_BINS[2] + kk * N_BB_BINS[3] + ll+1;
						Size sortedrotno = packed_rotno_2_sorted_rotno_( jp1kp1lp1, ii );
						PackedDunbrackRotamer< T, N > & rot = rotamers_( jp1kp1lp1, sortedrotno );
						rot.n_derivs()[ 2 ] = rotEspline.get_dsecoz()(  jj, kk, ll  );
						rot.n_derivs()[ 3 ] = rotEspline.get_dsecoy()(  jj, kk, ll );
						rot.n_derivs()[ 4 ] = rotEspline.get_dsecoyz()( jj, kk, ll  );
						rot.n_derivs()[ 5 ] = rotEspline.get_dsecox()(  jj, kk, ll );
						rot.n_derivs()[ 6 ] = rotEspline.get_dsecoxz()(  jj, kk, ll  );
						rot.n_derivs()[ 7 ] = rotEspline.get_dsecoxy()(  jj, kk, ll );
						rot.n_derivs()[ 8 ] = rotEspline.get_dsecoxyz()( jj, kk, ll  );
					}
				}
			}
		}
	} else {
		for ( Size ii = 1; ii <= n_packed_rots(); ++ii ) {
			using namespace numeric;
			using namespace numeric::interpolation::spline;

			utility_exit_with_message( "Polycubic interpolation for N >= 4 doesn't yet work -- there are still some bugs to work out.  Please contact Vikram K. Mulligan and Andy Watkins to complain about this." );

			PolycubicSpline< N > rotEspline;
			utility::fixedsizearray1< Size, N > n_dims;
			for ( Size i = 1; i <= N; ++i ) n_dims[ i ] = N_BB_BINS[i];
			MathNTensor< Real, N > energy_vals(n_dims, 0.0);

			utility::fixedsizearray1< Size, (N+1) > bb_bin( 1 );
			utility::fixedsizearray1< Size, (N+1) > bb_bin_maxes( 1 );
			for ( Size i = 1; i <= N; ++i ) bb_bin_maxes[i] = N_BB_BINS[i];

			while ( bb_bin[ N + 1 ] == 1 ) {

				Size bb_rot_index = make_index< N >( N_BB_BINS, bb_bin );
				Size sortedrotno = packed_rotno_2_sorted_rotno_( bb_rot_index, ii );
				PackedDunbrackRotamer< T, N > & rot = rotamers_( bb_rot_index, sortedrotno );
				core::Real rotamer_energy = -std::log( core::Real(rot.rotamer_probability()) );
				rot.n_derivs()[ 1 ] = rotamer_energy;
				utility::vector1< Size > indices;
				for ( Size i = 1; i <= N; ++i ) indices.push_back( bb_bin[ i ] - 1 );
				energy_vals( indices ) = rotamer_energy;

				increment_tensor_index_recursively(bb_bin, bb_bin_maxes, 1);
			}

			utility::fixedsizearray1< BorderFlag, N > periodic_boundary( e_Periodic );
			utility::fixedsizearray1< Real, N > start_vals;
			utility::fixedsizearray1< Real, N > deltas( 10.0 );
			for ( Size s = 1; s <= N; ++s ) deltas[ s ] = BB_BINRANGE[ s ];
			utility::fixedsizearray1< bool, N > lincont( false );
			utility::fixedsizearray1< std::pair< Real, Real >, N > unused( std::pair< Real, Real >( 10, 10 ) );
			rotEspline.train( periodic_boundary, start_vals, deltas, energy_vals, lincont, unused );

			for ( Size clear_i = 1; clear_i <= N+1; ++clear_i ) bb_bin[ clear_i ] = 1;

			while ( bb_bin[ N + 1 ] == 1 ) {

				Size bb_rot_index = make_index< N >( N_BB_BINS, bb_bin );
				Size sortedrotno = packed_rotno_2_sorted_rotno_( bb_rot_index, ii );
				PackedDunbrackRotamer< T, N > & rot = rotamers_( bb_rot_index, sortedrotno );

				utility::vector1< Size > indices;
				indices.reserve(N);
				for ( Size i = 1; i <= N; ++i ) indices.push_back( bb_bin[ i ] - 1 );
				for ( Size i = 1; i <= ( 1 << N ); ++i ) rot.n_derivs()[ i ] = rotEspline.get_deriv( i )( indices );

				//std::cout << "index=" << indices << std::endl; //DELETE ME LATER!
				increment_tensor_index_recursively( bb_bin, bb_bin_maxes, 1 );
			}
		}
	}
}

/// @brief Given a RotamericSingleResidueDunbrackLibrary of type T (probably core::Real) and with N backbone dihedrals,
/// a vector of coordinates in the backbone bins tensor, and a vector of sizes for the dimensions of the backbone bins
/// tensor, increment the coordinate in a manner that ensures that all bins can be iterated over.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
template< Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::increment_tensor_index_recursively(
	utility::fixedsizearray1< Size, (N+1) > & bb_bin,
	utility::fixedsizearray1< Size, (N+1) > const & bb_bin_maxes,
	core::Size const recursion_level
) const {
	bb_bin[recursion_level]++;
	if ( recursion_level == bb_bin.size() ) return;

	if ( bb_bin[recursion_level] > bb_bin_maxes[recursion_level] ) {
		for ( core::Size i(1); i<=bb_bin[recursion_level]; ++i ) {
			bb_bin[i] = 1;
		}
		increment_tensor_index_recursively( bb_bin, bb_bin_maxes, recursion_level + 1 );
	}
}

/// @details called only if the library is actually an RSRDL<T> object.  Derived classes
/// should not call this function or recurse.  Accounts for the statically allocated data
/// that's part of this class.
template < Size T, Size N >
Size RotamericSingleResidueDunbrackLibrary< T, N >::memory_usage_static() const
{
	return sizeof( RotamericSingleResidueDunbrackLibrary< T, N > );
}

/// @details Measures the amount of dynamically allocated data in this class.  Must recurse to parent
/// to count parent's dynamically allocated data.
template < Size T, Size N >
Size RotamericSingleResidueDunbrackLibrary< T, N >::memory_usage_dynamic() const
{
	Size total = parent::memory_usage_dynamic(); // recurse to parent.
	total += rotamers_.size() * sizeof( PackedDunbrackRotamer< T, N > );
	total += packed_rotno_2_sorted_rotno_.size() * sizeof( Size ); // could make these shorts or chars!
	//total += max_rotprob_.size() * sizeof( DunbrackReal );
	return total;
}


/// @brief Given chi values and a pose residue, return indices specifying the nearest rotamer well centre.
/// @details This version uses a Voronoi-inspired algorithm: the nearest rotamer well to the input chi values, in wraparound
/// angle space, is returned.  This is compatible with non-canonicals.  Might be slightly slower.
/// @author Vikram K. Mulligan (vmullig@uw.edu).
template< Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::get_rotamer_from_chi_static_voronoi(
	ChiVector const & chi,
	core::Size & rot,
	core::conformation::Residue const &rsd,
	core::pose::Pose const &pose
) const {
	//Get current backbone bin:
	utility::fixedsizearray1< core::Real, N > bbs( get_IVs_from_rsd( rsd, pose ) );
	if ( rsd.type().is_mirrored_type() ) {
		for ( core::Size i(1); i<=N; ++i ) bbs[i] *= -1.0;
	}
	//std::cout << "**-**-**-** rot=" << rot << "\tbbs=" << bbs << std::endl; //DELETE ME

	utility::fixedsizearray1< core::Size, N > bb_bin;
	get_bb_bins( bbs, bb_bin );
	core::Size const bb_index( make_index< N >( N_BB_BINS, bb_bin ) );

	//std::cout << "**-**-**-** bb_bin=" << bb_bin << "\tbb_index=" << bb_index << std::endl; //DELETE ME

	//Loop through all rotamers in this backbone bin, and get closest to current chi.
	core::Real dist_sq(0.0), lowest_dist_sq(0.0);
	PackedDunbrackRotamer< T, N > const * lowest_rotamer( nullptr ); //Temporary use of raw pointer is simplest here.  Note that ownership of object pointed to is not an issue.
	bool first(true);
	for ( core::Size irot(1), irotmax(rotamers_.size2()); irot<=irotmax; ++irot ) {
		PackedDunbrackRotamer< T, N > const &cur_rot( rotamers_( bb_index, irot ) );
		//std::cout << "**-**-**-** Considering packed_rotno=" << cur_rot.packed_rotno() << std::endl; //DELETE ME
		dist_sq = 0;
		for ( core::Size ichi(1); ichi<=T; ++ichi ) {
			dist_sq += pow( basic::subtract_degree_angles( cur_rot.chi_mean(ichi), chi[ichi] ), 2 );
		}
		if ( first || dist_sq < lowest_dist_sq ) {
			lowest_rotamer = &cur_rot;
			lowest_dist_sq = dist_sq;
			first = false;
		}
	}
	debug_assert( lowest_rotamer != nullptr ); //Should be guaranteed true.

	// Get the rotamer bins, and store them in rot:
	rot = lowest_rotamer->packed_rotno();
	//std::cout << "**-**-**-** lowest_chimean(1)=" << lowest_rotamer->chi_mean(1) << "\tpacked_rotno=" << lowest_rotamer->packed_rotno() << "\trot=" << rot << std::endl; //DELETE ME
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::get_rotamer_from_chi(
	ChiVector const & chi,
	RotVector & rot
) const
{
	Size4 rot4;
	get_rotamer_from_chi_static( chi, rot4 );
	rot.resize( chi.size() );
	for ( Size ii = 1; ii <= T; ++ii ) rot[ ii ] = rot4[ ii ];
	for ( Size ii = T+1; ii <= chi.size(); ++ii ) rot[ ii ] = 0;
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::get_rotamer_from_chi_static(
	ChiVector const & chi,
	Size4 & rot
) const
{
	Real4 chi4;
	for ( Size ii = 1; ii <= T; ++ii ) chi4[ ii ] = chi[ ii ];
	get_rotamer_from_chi_static( chi4, rot );
}

template < Size T, Size N >
void
RotamericSingleResidueDunbrackLibrary< T, N >::get_rotamer_from_chi_static(
	Real4 const & chi,
	Size4 & rot
) const
{
	if ( dun02() ) {
		core::chemical::AA aa2( aa() );
		if ( core::chemical::is_canonical_D_aa( aa2 ) ) aa2 = core::chemical::get_L_equivalent( aa2 );
		if ( canonical_aa_ && is_canonical_L_aa_or_gly( aa2 ) ) { rotamer_from_chi_02( chi, aa2, T, rot ); return; }
	}

	debug_assert( chi.size() >= T );

	/// compiler will unroll this loop
	for ( Size ii = 1; ii <= T; ++ii ) rot[ ii ] = bin_rotameric_chi( basic::periodic_range( chi[ ii ], 360 ), ii );
}


} // namespace dunbrack
} // namespace scoring
} // namespace core

#endif // INCLUDED_core_pack_dunbrack_RotamericSingleResidueDunbrackLibrary_TMPL_HH
