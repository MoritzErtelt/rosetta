// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/energy_methods/GaussianOverlapEnergy.cc
/// @brief  Gaussian Overlap Energy
/// @author Ben Borgo (bborgo@genetics.wustl.edu)


#include <core/conformation/Residue.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/energy_methods/GaussianOverlapEnergy.hh>
#include <core/energy_methods/GaussianOverlapEnergyCreator.hh>
//#include <core/scoring/ScoringManager.hh>
#include <core/scoring/EnergyMap.hh>
#include <numeric/NumericTraits.hh>
#include <core/chemical/AA.hh>
#include <core/chemical/VariantType.hh>


//Auto Headers
#include <core/id/AtomID.fwd.hh>


#define DIST_TH 6.0
//#define TETHER_WT 10.0

namespace core {
namespace energy_methods {


GaussianOverlapEnergy::~GaussianOverlapEnergy() = default;


/// @details This must return a fresh instance of the GaussianOverlapEnergy class,
/// never an instance already in use
core::scoring::methods::EnergyMethodOP
GaussianOverlapEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const &
) const {
	return utility::pointer::make_shared< GaussianOverlapEnergy >();
}

core::scoring::ScoreTypes
GaussianOverlapEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( gauss );
	return sts;
}


GaussianOverlapEnergy::GaussianOverlapEnergy() :
	parent( utility::pointer::make_shared< GaussianOverlapEnergyCreator >() )
{
}


/// clone
core::scoring::methods::EnergyMethodOP
GaussianOverlapEnergy::clone() const
{
	return utility::pointer::make_shared< GaussianOverlapEnergy >();
}


inline bool count_atom( int const & atype ) {
	// return true;
	return (  (  1 <= atype && atype <=  6 ) // just carbons for now...
		|| ( 18 <= atype && atype <= 19 )
	);
}
/////////////////////////////////////////////////////////////////////////////
// scoring
/////////////////////////////////////////////////////////////////////////////


void
GaussianOverlapEnergy::residue_pair_energy(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	pose::Pose const & /*pose*/,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap & emap
) const
{
	using namespace core;
	using namespace conformation;

	Real score(0.0);

	if ( rsd1.seqpos() == rsd2.seqpos() ) return;

	chemical::AA const aa1( rsd1.aa() );
	chemical::AA const aa2( rsd2.aa() );
	if ( aa1 == chemical::aa_cys && aa2 == chemical::aa_cys &&
			rsd1.is_bonded( rsd2 ) && rsd1.polymeric_sequence_distance( rsd2 ) > 1 &&
			rsd1.has_variant_type( chemical::DISULFIDE ) && rsd2.has_variant_type( chemical::DISULFIDE ) ) return;
	if ( aa1 == chemical::aa_pro && aa2 == chemical::aa_pro ) return;

	for ( Size i = 1, i_end = rsd1.nheavyatoms(); i <= i_end; ++i ) {
		Vector const & i_xyz( rsd1.xyz(i) );
		for ( Size j = 1, j_end = rsd2.nheavyatoms(); j <= j_end; ++j ) {
			Vector const & j_xyz( rsd2.xyz(j) );

			Real const d2( i_xyz.distance_squared( j_xyz ) );

			if ( d2 > 150.0 ) continue;

			core::Real r1 = rsd1.atom_type(i).lj_radius();
			core::Real r2 = rsd2.atom_type(j).lj_radius();

			r1 = .92*r1; r2 = .92*r2;
			if ( d2 <= .5*(r1+r2) ) {
				score = 1000;
			} else if ( d2 <= .8*(r1+r2) ) {
				score = (r1+r2)/(d2*d2*d2*d2*sqrt(d2));
			} else {
				score = ((sqrt(numeric::NumericTraits<Real>::pi())*r1*r2)/(sqrt(r1*r1+r2*r2)))*(exp(-d2/(r1*r1+r2*r2)));
			}
			emap[ core::scoring::gauss ] += score; //This was incorrectly placed in the "else" block above.  --VKM, 26 Nov. 2017.
		}
	}
}

void
GaussianOverlapEnergy::eval_atom_derivative(
	id::AtomID const & /*id*/,
	pose::Pose const & /*pose*/,
	kinematics::DomainMap const &, // domain_map,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap const &,
	Vector & /*F1*/,
	Vector & /*F2*/
) const
{
	// noop?
}

void
GaussianOverlapEnergy::eval_intrares_energy(
	conformation::Residue const & /*rsd*/,
	pose::Pose const & /*pose*/,
	core::scoring::ScoreFunction const & /*sfxn*/,
	core::scoring::EnergyMap & /*emap*/
) const {
	// noop?
}


/// @brief GaussianOverlapEnergy distance cutoff set to the same cutoff used by EtableEnergy, for now
Distance
GaussianOverlapEnergy::atomic_interaction_cutoff() const
{
	return interaction_cutoff();
}

/// @details non-virtual accessor for speed; assumption: GaussianOverlapEnergy is not inherrited from.
Distance
GaussianOverlapEnergy::interaction_cutoff() const
{
	return DIST_TH;
}

/// @brief GaussianOverlapEnergy requires that Energies class maintains a core::scoring::TenANeighborGraph
void
GaussianOverlapEnergy::indicate_required_context_graphs( utility::vector1< bool > & /*context_graphs_required*/ ) const
{
}
core::Size
GaussianOverlapEnergy::version() const
{
	return 1; // Initial versioning
}


}
}
