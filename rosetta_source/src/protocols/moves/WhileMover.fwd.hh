// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/protocols/WhileMover.fwd.hh
/// @brief
/// @author

#ifndef INCLUDED_protocols_moves_WhileMover_fwd_hh
#define INCLUDED_protocols_moves_WhileMover_fwd_hh

#include <utility/pointer/owning_ptr.hh>

// Package headers

namespace protocols {
namespace moves {

class WhileMover;
typedef utility::pointer::owning_ptr< WhileMover > WhileMoverOP;
typedef utility::pointer::owning_ptr< WhileMover const > WhileMoverCOP;

class PoseCondition;
typedef utility::pointer::owning_ptr< PoseCondition > PoseConditionOP;
typedef utility::pointer::owning_ptr< PoseCondition const > PoseConditionCOP;

} // moves
} // protocols


#endif
