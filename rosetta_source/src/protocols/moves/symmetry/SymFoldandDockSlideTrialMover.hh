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


#ifndef INCLUDED_protocols_moves_symmetry_SymFoldandDockSlideTrialMover_hh

#define INCLUDED_protocols_moves_symmetry_SymFoldandDockSlideTrialMover_hh

// Unit headers
#include <protocols/moves/symmetry/SymFoldandDockSlideTrialMover.fwd.hh>
#include <protocols/moves/Mover.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/conformation/symmetry/SymmetricConformation.fwd.hh>
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>

// Utility Headers

namespace protocols {
namespace moves {
namespace symmetry {
///////////////////////////////////////////////////////////////////////////////
class SymFoldandDockSlideTrialMover : public Mover
{
public:

typedef core::conformation::symmetry::SymmetricConformation SymmetricConformation;
typedef core::conformation::symmetry::SymmetryInfo SymmetryInfo;

public:

	// default constructor
	SymFoldandDockSlideTrialMover();

	//SymFoldandDockSlideTrialMover( std::string const & );

	~SymFoldandDockSlideTrialMover();

	void apply( core::pose::Pose & pose );
	virtual std::string get_name() const;

};

} // symmetry
} // moves
} // rosetta
#endif
