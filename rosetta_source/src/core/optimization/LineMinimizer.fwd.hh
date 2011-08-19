// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/optimization/LineMinimizer.fwd.hh
/// @brief  core::optimization::LineMinimizer forward declarations
/// @author Jim Havranek


#ifndef INCLUDED_core_optimization_LineMinimizer_fwd_hh
#define INCLUDED_core_optimization_LineMinimizer_fwd_hh

#include <utility/pointer/owning_ptr.hh>


namespace core {
namespace optimization {


// Forward
class LineMinimizationAlgorithm;

typedef utility::pointer::owning_ptr< LineMinimizationAlgorithm > LineMinimizationAlgorithmOP;
typedef utility::pointer::owning_ptr< LineMinimizationAlgorithm const > LineMinimizationAlgorithmCOP;

} // namespace optimization
} // namespace core


#endif // INCLUDED_core_optimization_LineMinimizer_FWD_HH
