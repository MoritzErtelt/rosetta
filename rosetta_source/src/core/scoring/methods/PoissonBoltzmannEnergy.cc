// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/methods/PoissonBoltzmannEnergy.cc
/// @brief  Ramachandran energy method class implementation
/// @author Phil Bradley
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

// Unit Headers
#include <core/scoring/methods/PoissonBoltzmannEnergy.hh>
#include <core/scoring/methods/PoissonBoltzmannEnergyCreator.hh>

// Package Headers
#include <core/scoring/PoissonBoltzmannPotential.hh>
#include <core/scoring/ScoringManager.hh>
#include <core/scoring/EnergyMap.hh>
#include <core/scoring/OneToAllEnergyContainer.hh>
#include <core/scoring/Energies.hh>
#include <core/scoring/methods/Methods.hh>

// Project headers
#include <core/conformation/Residue.hh>
#include <core/pose/Pose.hh>

// Utility headers
#include <ObjexxFCL/format.hh>
#include <numeric/conversions.hh>
#include <basic/Tracer.hh>

static basic::Tracer TR("core.scoring.methods.PoissonBoltzmannEnergy");

namespace core {
namespace scoring {
namespace methods {


/// @details This must return a fresh instance of the PoissonBoltzmannEnergy class,
/// never an instance already in use
methods::EnergyMethodOP
PoissonBoltzmannEnergyCreator::create_energy_method(
	methods::EnergyMethodOptions const &
) const {
	return new PoissonBoltzmannEnergy;
}

ScoreTypes
PoissonBoltzmannEnergyCreator::score_types_for_method() const {
	ScoreTypes sts;
	sts.push_back( PB_elec );
	return sts;
}


/// ctor
PoissonBoltzmannEnergy::PoissonBoltzmannEnergy() :
	parent( new PoissonBoltzmannEnergyCreator )
{
	potential_is_loaded_ = core::scoring::get_PB_potential().isLoaded();
	fixed_residue_ = 1;
}

methods::LongRangeEnergyType
PoissonBoltzmannEnergy::long_range_type() const { return methods::PB_elec_lr; }

void
PoissonBoltzmannEnergy::setup_for_scoring(
								  pose::Pose & pose,
								  ScoreFunction const &
								  ) const {
	using namespace methods;
	
	// Do we have a map?
	if (!potential_is_loaded_) {
		utility_exit_with_message("No potential loaded.");
	}
	
	// make sure the root of the FoldTree is a virtual atom and is followed by a jump
	// if not, emit warning
	//kinematics::Edge const &root_edge ( *pose.fold_tree().begin() );
	//int virt_res_idx = root_edge.start();
	//conformation::Residue const &root_res( pose.residue( virt_res_idx ) );
	
	//pose_is_proper = true;
	//if (root_res.aa() != core::chemical::aa_vrt || root_edge.label() < 0) {
		//utility_exit_with_message("Fold tree is not set properly for density scoring!");
		//TR.Error << "Fold tree is not set properly for density scoring!" << std::endl;
		//pose_is_proper = false;
	//}
	
	//
	// create LR energy container
	LongRangeEnergyType const & lr_type( long_range_type() );
	Energies & energies( pose.energies() );
	bool create_new_lre_container( false );
	
	if ( energies.long_range_container( lr_type ) == 0 ) {
		create_new_lre_container = true;
	} else {
		LREnergyContainerOP lrc = energies.nonconst_long_range_container( lr_type );
		OneToAllEnergyContainerOP dec( static_cast< OneToAllEnergyContainer * > ( lrc.get() ) );
		// make sure size or root did not change
		if ( dec->size() != pose.total_residue() ) {
			create_new_lre_container = true;
		}
	}
	
	if ( create_new_lre_container ) {
		TR << "Creating new one-to-all energy container (" << pose.total_residue() << ")" << std::endl;
		LREnergyContainerOP new_dec = new OneToAllEnergyContainer( fixed_residue_, pose.total_residue(), PB_elec );
		energies.set_long_range_container( lr_type, new_dec );
	}
	
	// allocate space for per-AA stats
	//int nres = pose.total_residue();
	//core::scoring::electron_density::getDensityMap().set_nres( nres );
}


/// clone
EnergyMethodOP
PoissonBoltzmannEnergy::clone() const
{
	return new PoissonBoltzmannEnergy;
}

/////////////////////////////////////////////////////////////////////////////
// methods
/////////////////////////////////////////////////////////////////////////////

///
void
PoissonBoltzmannEnergy::residue_energy(
	conformation::Residue const & rsd,
	pose::Pose const &,
	EnergyMap & emap
) const
{
	Real PB_score_residue, PB_score_backbone, PB_score_sidechain;
	core::scoring::get_PB_potential().eval_PB_energy_residue( rsd, PB_score_residue, PB_score_backbone, PB_score_sidechain );
	emap[ PB_elec ] += PB_score_sidechain;
}

bool PoissonBoltzmannEnergy::defines_residue_pair_energy(
												 pose::Pose const & pose,
												 Size res1,
												 Size res2
												 ) const {
	return ( res1 == fixed_residue_ || res2 == fixed_residue_ );
}

void
PoissonBoltzmannEnergy::eval_intrares_energy(
									 conformation::Residue const &,
									 pose::Pose const &,
									 ScoreFunction const &,
									 EnergyMap &
									 ) const {
	return;
}
	
void
PoissonBoltzmannEnergy::residue_pair_energy(
									conformation::Residue const & rsd1,
									conformation::Residue const & rsd2,
									pose::Pose const & pose,
									ScoreFunction const &,
									EnergyMap & emap
									) const {
	//check fixed_residue_
	conformation::Residue const &rsd (rsd1.seqpos() == fixed_residue_? rsd2 : rsd1 );
	
	Real PB_score_residue, PB_score_backbone, PB_score_sidechain;
	core::scoring::get_PB_potential().eval_PB_energy_residue( rsd, PB_score_residue, PB_score_backbone, PB_score_sidechain );
	emap[ PB_elec ] += PB_score_sidechain;
}
	
/// @brief Energy is context independent and thus indicates that no context graphs need to
/// be maintained by class Energies
void
PoissonBoltzmannEnergy::indicate_required_context_graphs(
	utility::vector1< bool > & /*context_graphs_required*/
)
const
{}
	
core::Size
PoissonBoltzmannEnergy::version() const
{
	return 1; // Initial versioning
}


} // methods
} // scoring
} // core

