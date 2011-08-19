// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file
/// @brief  fwd headers for ns loops
/// @author Oliver Lange


#ifndef INCLUDED_protocols_loops_SlidingWindowLoopClosure_fwd_hh
#define INCLUDED_protocols_loops_SlidingWindowLoopClosure_fwd_hh


// Utility headers
//#include <utility/pointer/access_ptr.fwd.hh>
#include <utility/pointer/owning_ptr.fwd.hh>

namespace protocols {
namespace loops {

// Forward
//class BaseJumpSetup;
class SlidingWindowLoopClosure;

// Types
typedef  utility::pointer::owning_ptr< SlidingWindowLoopClosure >  SlidingWindowLoopClosureOP;
typedef  utility::pointer::owning_ptr< SlidingWindowLoopClosure const >  SlidingWindowLoopClosureCOP;



} // namespace loops
} // namespace protocols

#endif
