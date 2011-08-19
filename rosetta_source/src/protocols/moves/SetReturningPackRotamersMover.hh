// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file SetReturningPackRotamersMover.hh
/// @brief A PackRotamers mover which returns a set of packed poses for when ndruns/nloop is in use.
/// @author Ron Jacak

#ifndef INCLUDED_protocols_moves_SetReturningPackRotamersMover_hh
#define INCLUDED_protocols_moves_SetReturningPackRotamersMover_hh

// Unit headers
#include <protocols/moves/SetReturningPackRotamersMover.fwd.hh>
#include <protocols/moves/PackRotamersMover.hh> // needed b/c we are extending from PackRotamersMover

// Project headers
#include <core/types.hh>
#include <core/pack/task/PackerTask.fwd.hh> // for PT COP
// AUTO-REMOVED #include <core/pose/Pose.hh>  // needed because one function takes a vector of Pose objects, not a vector of Pose references or Pose OP's
#include <core/scoring/ScoreFunction.fwd.hh> // for SF COP

#include <core/pose/Pose.fwd.hh>

#include <utility/vector1.hh>


//Auto Headers
#include <iostream>


namespace protocols {
namespace moves {


class SetReturningPackRotamersMover : public protocols::moves::PackRotamersMover {

public:
	SetReturningPackRotamersMover( Size ndruns );
	// custom constructor
	SetReturningPackRotamersMover( core::scoring::ScoreFunctionCOP scorefxn, core::pack::task::PackerTaskCOP task, core::Size ndruns );

	virtual void apply( core::pose::Pose & pose );
	virtual std::string get_name() const;
	void get_repacked_poses( utility::vector1< core::pose::Pose > & v );
	void output_repacked_poses( std::string filename_prefix );

private:
	utility::vector1< core::pose::Pose > repacked_poses_;
	core::Size ndruns_;

};

} // moves
} // protocols


#endif
