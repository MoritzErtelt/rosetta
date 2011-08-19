// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file TrialMover
/// @brief performs a move and accepts it according to Monte Carlo accept/reject criterion.
/// @author Monica Berrondo


#ifndef INCLUDED_protocols_moves_DME_FilterMover_hh
#define INCLUDED_protocols_moves_DME_FilterMover_hh

// Package headers
// #include <protocols/moves/MinMover.hh>
// #include <protocols/moves/MonteCarlo.fwd.hh>
// #include <protocols/moves/MonteCarlo.hh>
#include <protocols/moves/Mover.hh>
// #include <protocols/moves/MoverStatistics.hh>

#include <core/types.hh>

#include <core/pose/Pose.fwd.hh>

// ObjexxFCL Headers

// C++ Headers
#include <map>
#include <string>

// Utility Headers
// AUTO-REMOVED #include <numeric/random/random.hh>

//Auto Headers
#include <iostream>


namespace protocols {
namespace moves {


class DME_FilterMover : public Mover {

public:

	typedef core::Real Real;
	typedef core::Size Size;

public:

	// constructor with arguments
	DME_FilterMover( MoverOP my_mover, Real const dme_threshold, Size const max_tries ):
		Mover( my_mover->type()+"DME_Filter" ),
		my_mover_( my_mover ),
		dme_threshold_( dme_threshold ),
		max_tries_( max_tries )
	{}

	virtual
	void
	apply( core::pose::Pose & pose );

	virtual std::string get_name() const;

private:

	MoverOP my_mover_;
	Real const dme_threshold_;
	Size const max_tries_;

};

} // moves
} // protocols


#endif
