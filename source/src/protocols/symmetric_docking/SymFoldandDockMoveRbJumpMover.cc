// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file MinMover.cc
/// @brief
/// @author Ingemar Andre

// Unit headers
#include <protocols/symmetric_docking/SymFoldandDockMoveRbJumpMover.hh>
#include <protocols/simple_moves/symmetry/SetupForSymmetryMover.hh>

// Package headers
#include <core/pose/symmetry/util.hh>

#include <protocols/symmetric_docking/SymFoldandDockCreators.hh>

#include <utility/tag/Tag.hh>
#include <basic/datacache/DataMap.hh>
#include <protocols/rosetta_scripts/util.hh>

// Utility Headers
#include <basic/Tracer.hh>
#include <utility/vector1.hh>


namespace protocols {
namespace symmetric_docking {

static basic::Tracer TR("protocols.moves.symmetry.SymFoldandDockMoveRbJumpMover");


SymFoldandDockMoveRbJumpMover::SymFoldandDockMoveRbJumpMover()
	: Mover("SymFoldandDockMoveRbJumpMover")
{}

void
SymFoldandDockMoveRbJumpMover::apply( core::pose::Pose & pose )
{
	using namespace core::conformation::symmetry;
	protocols::simple_moves::symmetry::SetupForSymmetryMover setup;
	setup.apply( pose );
	core::pose::symmetry::find_new_symmetric_jump_residues( pose );
}

std::string
SymFoldandDockMoveRbJumpMover::get_name() const {
	return "SymFoldandDockMoveRbJumpMover";
}

void
SymFoldandDockMoveRbJumpMover::parse_my_tag( 
		utility::tag::TagCOP const,
		basic::datacache::DataMap & ,
		protocols::filters::Filters_map const &,
		protocols::moves::Movers_map const &,
		core::pose::Pose const & )
{
}

///

std::string
SymFoldandDockMoveRbJumpMoverCreator::keyname() const {
    return SymFoldandDockMoveRbJumpMoverCreator::mover_name();
}

protocols::moves::MoverOP
SymFoldandDockMoveRbJumpMoverCreator::create_mover() const {
    return new SymFoldandDockMoveRbJumpMover();
}

std::string
SymFoldandDockMoveRbJumpMoverCreator::mover_name() {
    return "SymFoldandDockMoveRbJumpMover";
}

} // symmetric_docking
} // protocols
