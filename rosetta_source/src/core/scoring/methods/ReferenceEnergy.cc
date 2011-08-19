// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/methods/ReferenceEnergy.hh
/// @brief  Reference energy method implementation
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

// Unit headers
#include <core/scoring/methods/ReferenceEnergy.hh>
#include <core/scoring/methods/ReferenceEnergyCreator.hh>

// Package headers
#include <core/scoring/EnergyMap.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>

// Project headers
#include <core/chemical/AA.hh>
#include <core/conformation/Residue.hh>

namespace core {
namespace scoring {
namespace methods {


/// @details This must return a fresh instance of the ReferenceEnergy class,
/// never an instance already in use
methods::EnergyMethodOP
ReferenceEnergyCreator::create_energy_method(
	methods::EnergyMethodOptions const & options
) const {
	if ( options.has_method_weights( ref ) ) {
		return new ReferenceEnergy( options.method_weights( ref ) );
	} else {
		return new ReferenceEnergy;
	}
}

ScoreTypes
ReferenceEnergyCreator::score_types_for_method() const {
	ScoreTypes sts;
	sts.push_back( ref );
	return sts;
}



ReferenceEnergy::ReferenceEnergy() :
	parent( new ReferenceEnergyCreator )
{}

ReferenceEnergy::ReferenceEnergy( utility::vector1< Real > const & aa_weights_in ):
	parent( new ReferenceEnergyCreator ),
	aa_weights_( aa_weights_in )
{}

ReferenceEnergy::~ReferenceEnergy() {}

EnergyMethodOP
ReferenceEnergy::clone() const
{
	return new ReferenceEnergy( aa_weights_ );
}

/// This is a terrible terrible terrible hack that will do for now.
void
ReferenceEnergy::residue_energy(
	conformation::Residue const & rsd,
	pose::Pose const &,
	EnergyMap & emap
) const
{
	// ignore scoring residues which have been marked as "REPLONLY" residues (only the repulsive energy will be calculated)
	if ( rsd.has_variant_type( "REPLONLY" ) ){
			return;
	}

	using namespace chemical;
	if ( !aa_weights_.empty() ) {
		///
		AA const & aa( rsd.aa() );
		if ( Size(aa) > aa_weights_.size() ) return;
		emap[ ref ] += aa_weights_[ aa ];
// 		if ( rsd.is_DNA() ) {
// 			std::cout << "using dna refE " << aa_weights_[aa] << std::endl;
// 		}
		return;
	}

	/// else -- use the default reference weights from r++
	if ( rsd.type().aa() > num_canonical_aas ) return;

	/* Stolen from rosetta++; negated so that Wref is a positive number
		Waa( aa_ala ) = -0.16;  //ALA,1
		Waa( aa_cys ) = -1.70;  //CYS,2
		Waa( aa_asp ) =  0.67;  //ASP,3
		Waa( aa_glu ) =  0.81;  //GLU,4
		Waa( aa_phe ) = -0.63;  //PHE,5
		Waa( aa_gly ) =  0.17;  //GLY,6
		Waa( aa_his ) = -0.56;  //HIS,7
		Waa( aa_ile ) = -0.24;  //ILE,8
		Waa( aa_lys ) =  0.65;  //LYS,9
		Waa( aa_leu ) =  0.10;  //LEU,10
		Waa( aa_met ) =  0.34;  //MET,11
		Waa( aa_asn ) =  0.89;  //ASN,12
		Waa( aa_pro ) = -0.02;  //PRO,13
		Waa( aa_gln ) =  0.97;  //GLN,14
		Waa( aa_arg ) =  0.98;  //ARG,15
		Waa( aa_ser ) =  0.37;  //SER,16
		Waa( aa_thr ) =  0.27;  //THR,17
		Waa( aa_val ) = -0.29;  //VAL,18
		Waa( aa_trp ) = -0.91;  //TRP,19
		Waa( aa_tyr ) = -0.51;  //TYR,20
	*/

	switch ( rsd.type().aa() ) {
		case aa_ala: emap[ ref ] +=  0.16; break;
		case aa_cys: emap[ ref ] +=  1.70; break;
		case aa_asp: emap[ ref ] +=  -0.67; break;
		case aa_glu: emap[ ref ] +=  -0.81; break;
		case aa_phe: emap[ ref ] +=  0.63; break;
		case aa_gly: emap[ ref ] +=  -0.17; break;
		case aa_his: emap[ ref ] +=  0.56; break;
		case aa_ile: emap[ ref ] +=  0.24; break;
		case aa_lys: emap[ ref ] +=  -0.65; break;
		case aa_leu: emap[ ref ] +=  -0.10; break;
		case aa_met: emap[ ref ] +=  -0.34; break;
		case aa_asn: emap[ ref ] +=  -0.89; break;
		case aa_pro: emap[ ref ] +=  0.02; break;
		case aa_gln: emap[ ref ] +=  -0.97; break;
		case aa_arg: emap[ ref ] +=  -0.98; break;
		case aa_ser: emap[ ref ] +=  -0.37; break;
		case aa_thr: emap[ ref ] +=  -0.27; break;
		case aa_val: emap[ ref ] +=  0.29; break;
		case aa_trp: emap[ ref ] +=  0.91; break;
		case aa_tyr: emap[ ref ] +=  0.51; break;
		default:
			utility_exit();
		break;
	}

}


Real
ReferenceEnergy::eval_dof_derivative(
	id::DOF_ID const &,
	id::TorsionID const &,
	pose::Pose const &,
	ScoreFunction const &,
	EnergyMap const &
) const
{
	return 0.0;
}

/// @brief ReferenceEnergy is context independent; indicates that no
/// context graphs are required
void
ReferenceEnergy::indicate_required_context_graphs( utility::vector1< bool > & ) const
{}
core::Size
ReferenceEnergy::version() const
{
	return 1; // Initial versioning
}


} // methods
} // scoring
} // core

