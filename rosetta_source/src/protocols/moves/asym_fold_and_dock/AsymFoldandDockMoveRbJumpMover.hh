// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file
/// @brief
/// @author Ingemar Andre


#ifndef INCLUDED_protocols_moves_asym_fold_and_dock_AsymFoldandDockMoveRbJumpMover_hh

#define INCLUDED_protocols_moves_asym_fold_and_dock_AsymFoldandDockMoveRbJumpMover_hh

// Unit headers
#include <protocols/moves/asym_fold_and_dock/AsymFoldandDockMoveRbJumpMover.fwd.hh>
#include <protocols/moves/Mover.hh>
#include <core/pose/Pose.fwd.hh>

// Utility Headers

namespace protocols {
namespace moves {
namespace asym_fold_and_dock {
///////////////////////////////////////////////////////////////////////////////
class AsymFoldandDockMoveRbJumpMover : public Mover
{
public:

	// default constructor

	AsymFoldandDockMoveRbJumpMover( core::Size chain_start );

	~AsymFoldandDockMoveRbJumpMover(){}

	void apply( core::pose::Pose & pose );
	void find_new_jump_residue( core::pose::Pose & pose );
	virtual std::string get_name() const;

private:
	core::Size chain_start_;

};

}
} // asym_fold_and_dock
} // rosetta
#endif
