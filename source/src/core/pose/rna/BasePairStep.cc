// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/pose/rna/BasePairStep.cc
/// @brief
/// @details
/// @author Rhiju Das, rhiju@stanford.edu


#include <core/pose/rna/BasePairStep.hh>
#include <core/types.hh>
#include <utility/exit.hh>

#include <ostream>

namespace core {
namespace pose {
namespace rna {

//Constructor
BasePairStep::BasePairStep( Size const i, Size const i_next,
	Size const j, Size const j_next ){
	runtime_assert( i_next == i+1 );
	// runtime_assert( j_next == j+1 );
	base_pair_step_ = std::make_pair( std::make_pair( i, i_next ), std::make_pair( j, j_next ) );
}

//Destructor
BasePairStep::~BasePairStep() = default;

std::ostream &
operator <<( std::ostream & os, BasePairStep const & bps ){
	os << bps.base_pair_step_.first.first << "-" << bps.base_pair_step_.first.second << " " << bps.base_pair_step_.second.first << "-" << bps.base_pair_step_.second.second;
	return os;
}

} //denovo
} //rna
} //protocols
