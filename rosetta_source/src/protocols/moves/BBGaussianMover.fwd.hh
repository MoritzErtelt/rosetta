/// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/moves/BBGaussianMover.fwd.hh
/// @brief  Gaussian Perturbation to backbone
/// @author Yuan Liu (wendao@u.washington.edu)

#ifndef INCLUDED_protocols_moves_BBGaussianMover_fwd_hh
#define INCLUDED_protocols_moves_BBGaussianMover_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace moves {

class BBGaussianMover;
typedef utility::pointer::owning_ptr< BBGaussianMover > BBGaussianMoverOP;
typedef utility::pointer::owning_ptr< BBGaussianMover const > BBGaussianMoverCOP;

class BBG8T3AMover;
typedef utility::pointer::owning_ptr< BBG8T3AMover > BBG8T3AMoverOP;
typedef utility::pointer::owning_ptr< BBG8T3AMover const > BBG8T3AMoverCOP;

}//namespace moves
}//namespace protocols

#endif

