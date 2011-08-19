// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/fragments/FragSet.hh
/// @brief  set of fragments
/// @author Oliver Lange
/// @date   Wed Aug 22 12:08:31 2007
///

#ifndef protocols_basic_moves_SymmetricFragmentMover_HH
#define protocols_basic_moves_SymmetricFragmentMover_HH

// Unit Headers
#include <protocols/basic_moves/SymmetricFragmentMover.fwd.hh>

// Package Headers
#include <protocols/basic_moves/SmoothFragmentMover.hh>
#include <protocols/basic_moves/FragmentMover.hh>
#include <core/fragment/Frame.fwd.hh>
// AUTO-REMOVED #include <core/fragment/FragData.fwd.hh>
#include <core/fragment/FragSet.hh>

// Project Headers
#include <core/kinematics/MoveMap.hh>
#include <core/pose/Pose.fwd.hh>
#include <protocols/moves/Mover.hh>

// ObjexxFCL Headers

// Utility headers
#include <utility/vector1.fwd.hh>

namespace protocols {
namespace basic_moves {
// might want to live in protocols::moves

///@brief A SymmetricFragmentMover that applies uniform sampling of fragments
class SymmetricFragmentMover : virtual public ClassicFragmentMover {
public:
	///@constructor
	SymmetricFragmentMover(
		core::fragment::FragSetCOP fragset,
		core::kinematics::MoveMapCOP movemap,
		Size symmetric_residue
	)	: ClassicFragmentMover( fragset, movemap, "SymmetricFragmentMover" ),
			image_start_( symmetric_residue )
	{};

	virtual
	bool
	apply_fragment(
		core::fragment::Frame const& frame,
		Size frag_num,
		core::kinematics::MoveMap const& movemap,
		core::pose::Pose &pose
	) const;


	virtual std::string get_name() const;

protected:
	///@brief alternative Constructor to be used by derived classes
	SymmetricFragmentMover(
		core::fragment::FragSetCOP fragset,
		core::kinematics::MoveMapCOP movemap,
		Size symmetric_residue,
		std::string type
	)	:
		ClassicFragmentMover( fragset, movemap, type ),
		image_start_( symmetric_residue )
	{}

private:
	Size image_start_;
};


///@brief A SymmetricFragmentMover that applies uniform sampling of fragments
class SmoothSymmetricFragmentMover : public SymmetricFragmentMover, public SmoothFragmentMover {
public:
	///@constructor
	SmoothSymmetricFragmentMover(
		core::fragment::FragSetCOP fragset,
		core::kinematics::MoveMapCOP movemap,
		FragmentCostOP cost,
		Size symmetric_residue
	)	:
		ClassicFragmentMover( fragset, movemap, "SmoothSymmetricFragmentMover" ),
		SymmetricFragmentMover( fragset, movemap, symmetric_residue, "SmoothSymmetricFragmentMover" ),
		SmoothFragmentMover( fragset, movemap, cost, "SmoothSymmetricFragmentMover" )
	{}

	virtual std::string get_name() const { return "SmoothSymmetricFragmentMover"; }

protected:
	///@brief alternative Constructor to be used by derived classes
	SmoothSymmetricFragmentMover(
		core::fragment::FragSetCOP fragset,
		core::kinematics::MoveMapCOP movemap,
		FragmentCostOP cost,
		std::string type,
		Size symmetric_residue
	)	:
		ClassicFragmentMover( fragset, movemap, type ), //virtual base class needs to be initialized explicitly
		SymmetricFragmentMover( fragset, movemap, symmetric_residue, type ),
		SmoothFragmentMover( fragset, movemap, cost, type )
	{}

private:
	Size image_start_;
};


} //basic_moves
} //protocls
#endif
