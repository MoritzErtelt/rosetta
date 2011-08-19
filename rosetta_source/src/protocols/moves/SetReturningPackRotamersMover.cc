// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file SetReturningPackRotamersMover.cc
/// @brief A pack mover which returns not just the best packer solution, but all nloop solutions.
/// @author Ron Jacak

// Unit headers
#include <protocols/moves/SetReturningPackRotamersMover.hh>
#include <protocols/moves/PackRotamersMover.hh>

// Project headers
#include <core/pack/task/PackerTask.hh>
#include <core/pack/task/TaskFactory.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <core/pose/Pose.hh> // need .hh because we create Pose object in apply()

#include <ObjexxFCL/format.hh>

#include <basic/Tracer.hh>

// AUTO-REMOVED #include <basic/options/option.hh>
// AUTO-REMOVED #include <basic/options/keys/packing.OptionKeys.gen.hh>

//Auto Headers
#include <utility/exit.hh>
#include <utility/options/keys/BooleanOptionKey.hh>


static basic::Tracer TR("protocols.moves.SetReturningPackRotamersMover");

// Utility Headers

namespace protocols {
namespace moves {

using namespace core;
using namespace scoring;
using namespace pack;


// default c'tor
SetReturningPackRotamersMover::SetReturningPackRotamersMover( Size ndruns ) :
	PackRotamersMover()
{
	repacked_poses_.resize( ndruns );
}

// custom c'tor
SetReturningPackRotamersMover::SetReturningPackRotamersMover(
	ScoreFunctionCOP scorefxn,
	task::PackerTaskCOP task,
	Size ndruns
) :
	PackRotamersMover( scorefxn, task, ndruns ),
	ndruns_(ndruns)
{
	repacked_poses_.resize( ndruns );
}


//
// @begin apply
//
// @brief
// The apply method for SetReturningPackRotamersMover.  Assumes that a valid score function and packer task were passed in.
// Still makes the best packed pose the returned pose, but also puts all of the repacked poses created in the member variable vector.
//
void
SetReturningPackRotamersMover::apply( pose::Pose & pose ) {

	// jec update_residue_neighbors() required to update EnergyGraph (ensures graph_state == GOOD) when calling Interface.cc
	pose.update_residue_neighbors();

	// guarantee of valid ScoreFunction and PackerTask postponed until now
	if ( this->score_function() == 0 ) {
		TR << "undefined ScoreFunction -- creating a default one" << std::endl;
		this->score_function( ScoreFunctionFactory::create_score_function( STANDARD_WTS ) );
	}

	// if present, task_factory_ always overrides/regenerates task_
	if ( this->task_factory() != 0 ) {
		this->task( (this->task_factory())->create_task_and_apply_taskoperations( pose ) );
	} else if ( this->task() == 0 ) {
		TR << "undefined PackerTask -- creating a default one" << std::endl;
		this->task( task::TaskFactory::create_packer_task( pose ) );
	}

	// in case PackerTask was not generated locally, verify compatibility with pose
	else runtime_assert( task_is_valid( pose ) );

	// get rotamers, energies
	this->setup( pose );

	core::PackerEnergy best_energy(0.);
	pose::Pose best_pose;
	best_pose = pose;

	// reset the size of ndruns_ to whatever was asked for since this Mover doesn't query the options system value
	ndruns_ = repacked_poses_.size();

	for ( Size run(1); run <= ndruns_; ++run ) {
		// run SimAnnealer
		core::PackerEnergy packer_energy( this->run( pose ) );
		// Real const score( scorefxn_( pose ) ); another option for deciding which is the 'best' result
		if ( run == 1 || packer_energy < best_energy ) {
			best_pose = pose;
			best_energy = packer_energy;
		}
		TR << "run " << run << ", packer energy: " << packer_energy << std::endl;
		repacked_poses_[ run ] = pose;

	}
	if ( ndruns_ > 1 )
		pose = best_pose;
}

std::string
SetReturningPackRotamersMover::get_name() const {
	return "SetReturningPackRotamersMover";
}


//
// @begin get_repacked_poses
//
// @brief
// Copies the poses in repacked_poses into the passed in vector reference.
//
void
SetReturningPackRotamersMover::get_repacked_poses( utility::vector1< core::pose::Pose > & v ) {
	v = repacked_poses_;
}

//
// @begin output_repacked_poses
//
// @brief
// Writes out all of the poses in the repacked poses vector to PDB files.
//
void
SetReturningPackRotamersMover::output_repacked_poses( std::string filename_prefix ) {

	for ( Size ii=1; ii <= repacked_poses_.size(); ++ii ) {
		// output repacked structure with the vector index in the name
		std::string filename = filename_prefix + "." + ObjexxFCL::fmt::I( 3, 3, ii ) + ".pdb";
		repacked_poses_[ ii ].dump_scored_pdb( filename, *(score_function()) );
	}

}


} // moves
} // protocols


