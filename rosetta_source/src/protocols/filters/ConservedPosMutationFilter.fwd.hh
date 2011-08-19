// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   /protocols/filters/ConservedPosMutationFilter.fwd.hh
/// @brief
/// @author Florian Richter (floric@u.washington.edu), may 2011


#ifndef INCLUDED_protocols_filters_ConservedPosMutationFilter_fwd_hh
#define INCLUDED_protocols_filters_ConservedPosMutationFilter_fwd_hh


// Utility headers
// AUTO-REMOVED #include <utility/pointer/access_ptr.fwd.hh>
#include <utility/pointer/owning_ptr.fwd.hh>


namespace protocols {
namespace filters {

// Forward
class ConservedPosMutationFilter;

// Types
typedef utility::pointer::owning_ptr< ConservedPosMutationFilter >  ConservedPosMutationFilterOP;
typedef utility::pointer::owning_ptr< ConservedPosMutationFilter const >  ConservedPosMutationFilterCOP;

} // namespace protocols
} // namespace filters

#endif
