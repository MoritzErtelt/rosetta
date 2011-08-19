// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file
/// @brief
/// @author

#ifndef INCLUDED_protocols_moves_DataMap_fwd_hh
#define INCLUDED_protocols_moves_DataMap_fwd_hh

#include <utility/pointer/owning_ptr.hh>

// Package headers

namespace protocols {
namespace moves {

class DataMap;

/// SJF do not define OP and COP for DataMap b/c it is not supposed to stick around
// in memory. In all applications, please register elements *from* the datamap rather
// than the entire map.
//typedef utility::pointer::owning_ptr< DataMap > DataMapOP;
//typedef utility::pointer::owning_ptr< DataMap const > DataMapCOP;

} // moves
} // protocols

#endif //INCLUDED_protocols_moves_DataMap_fwd_HH
