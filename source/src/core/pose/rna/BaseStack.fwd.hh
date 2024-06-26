// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/pose/rna/BaseStack.fwd.hh
/// @brief
/// @details
/// @author Rhiju Das, rhiju@stanford.edu


#ifndef INCLUDED_core_pose_rna_BaseStack_FWD_HH
#define INCLUDED_core_pose_rna_BaseStack_FWD_HH

#include <utility/pointer/owning_ptr.hh>
#include <utility/vector1.fwd.hh>

namespace core {
namespace pose {
namespace rna {

class BaseStack;
typedef utility::pointer::shared_ptr< BaseStack > BaseStackOP;
typedef utility::pointer::shared_ptr< BaseStack const > BaseStackCOP;
typedef utility::vector1 < BaseStack > RNA_BaseStackList;

} //rna
} //pose
} //core

#endif
