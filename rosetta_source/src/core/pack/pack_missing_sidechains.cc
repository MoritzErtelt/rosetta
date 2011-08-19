// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/pack/pack_missing_sidechains.cc
/// @brief  function to fix missing sidechains in input PDBs (especially those surface lysines that get sidechain-backbone clashes!)
/// @author Steven Lewis smlewi@gmail.com

// Unit headers
#include <core/pack/pack_missing_sidechains.hh>

// Package headers
#include <core/pack/pack_rotamers.hh>
#include <core/pack/task/PackerTask.hh>
#include <core/pack/task/TaskFactory.hh>

// Project headers
#include <core/chemical/AtomType.hh>
#include <core/chemical/ResidueType.hh>

// AUTO-REMOVED #include <core/id/AtomID_Mask.hh>

#include <core/pose/Pose.hh>

#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <basic/Tracer.hh>

//Auto Headers
#include <core/id/AtomID_Map.hh>


// ObjexxFCL headers


static basic::Tracer TR("core.pack.pack_missing_sidechains");

namespace core {
namespace pack {

///@details this function will run rotamer trials on sidechains with missing density.  It first sets up a PackerTask with repacking freedom for residues with sidechain missing atoms in the missing AtomID_Mask, then runs rotamer_trials.  This function is smart enough to ignore missing virtual atoms (type VIRT)
void
pack_missing_sidechains(
	core::pose::Pose & pose,
	core::id::AtomID_Mask const & missing
)
{
	//build a PackerTask to control rotamer_trials
	core::pack::task::PackerTaskOP task = core::pack::task::TaskFactory::create_packer_task( pose );
	task->initialize_from_command_line();
	task->restrict_to_repacking();
	utility::vector1_bool repackable( pose.total_residue() );
	bool any_missing(false);

	//set up the PackerTask
	//iterate over all sidechain atoms, and compare to the state of the input missing map.
	for( core::Size resid(1); resid <= pose.total_residue(); ++resid ){

		//iterate over all heavy sidechain atoms
		core::chemical::ResidueType const & restype(pose.residue_type(resid));
		for( core::Size atomno(restype.first_sidechain_atom()), end(restype.nheavyatoms()); atomno <= end; ++atomno){
			core::id::AtomID atomid(atomno, resid);
			//if the atom is missing and not a virtual atom...
			if( 	missing.get(atomid) &&
					restype.atom_type(atomno).name() != "VIRT"  &&
					restype.atom_type(atomno).name() != "ORBS"  &&
					restype.atom_type(atomno).name() != "LPbb"

			) {
				TR << "packing residue number " << resid << " because of missing atom number " << atomno << " atom name "
						<< restype.atom_name(atomno) << std::endl;
				repackable[resid] = true;
				any_missing = true;
				break; //we can stop now
			}
		}//for all sidechain heavyatoms
	}//all residues

	if(!any_missing) return; //return early - prevents weird "0 residues at 0 positions" packer calls

	//task is set up
	task->restrict_to_residues(repackable);

	core::scoring::ScoreFunctionOP sfxn = scoring::ScoreFunctionFactory::create_score_function( "standard", "score12" );
	(*sfxn)(pose); // structure must be scored before rotamer_trials can be called (?)
	//chu change from rotamer_trials to pack_rotamers
	core::pack::pack_rotamers( pose, *sfxn, task );
}//pack_missing_sidechains


} //namespace pack
} //namespace core

