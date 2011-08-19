// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/moves/util.hh
/// @author Christopher Miles (cmiles@uw.edu)

#ifndef PROTOCOLS_MOVES_UTIL_HH_
#define PROTOCOLS_MOVES_UTIL_HH_

// Utility headers
#include <utility/tag/Tag.fwd.hh>

// Project headers
#include <protocols/filters/Filter.hh>

// Package headers
#include <protocols/moves/Mover.hh>
#include <protocols/moves/DataMap.fwd.hh>

namespace protocols {
namespace moves {

/// @brief Searches <movers> for the named mover, returning it if it exists,
/// otherwise halts execution with an error message.
protocols::moves::MoverOP find_mover_or_die(const std::string& mover_name,
																						const utility::tag::TagPtr tag,
																						const protocols::moves::Movers_map& movers);

/// @brief Searches <filters> for the named filter, returning it if it exists,
/// otherwise halts execution with an error message.
protocols::filters::FilterOP find_filter_or_die(const std::string& filter_name,
																								const utility::tag::TagPtr tag,
																								const protocols::filters::Filters_map& filters);

}  // namespace moves
}  // namespace protocols

#endif  // PROTOCOLS_MOVES_UTIL_HH_
