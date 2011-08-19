// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/etable/count_pair/CountPairFunction.fwd.hh
/// @brief  Count pair base class forward declaration
/// @author Andrew Leaver-Fay (leaverfa@email.unc.edu)


#ifndef INCLUDED_core_scoring_etable_count_pair_CountPairFunction_fwd_hh
#define INCLUDED_core_scoring_etable_count_pair_CountPairFunction_fwd_hh

#include <utility/pointer/owning_ptr.hh>

namespace core {
namespace scoring {
namespace etable {
namespace count_pair {

class CountPairFunction;

typedef utility::pointer::owning_ptr< CountPairFunction > CountPairFunctionOP;
typedef utility::pointer::owning_ptr< CountPairFunction const > CountPairFunctionCOP;

} // namespace count_pair
} // namespace etable
} // namespace scoring
} // namespace core


#endif
