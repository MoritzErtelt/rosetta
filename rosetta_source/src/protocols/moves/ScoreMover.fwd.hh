// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/moves/ScoreMover.fwd.hh
/// @brief  ScoreMover forward declarations header
/// @author

#ifndef INCLUDED_protocols_moves_ScoreMover_fwd_hh
#define INCLUDED_protocols_moves_ScoreMover_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols{
namespace moves{

//Forwards and OP typedefs
class ScoreMover;
typedef utility::pointer::owning_ptr< ScoreMover > ScoreMoverOP;
typedef utility::pointer::owning_ptr< ScoreMover const > ScoreMoverCOP;

}//moves
}//protocols

#endif //INCLUDED_protocols_moves_ScoreMover_FWD_HH
