// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file core/scoring/hbonds/HBondOptions.fwd.hh
/// @brief forward header for HBondOptions class
/// @author Matthew O'Meara

#ifndef INCLUDED_core_scoring_hbonds_HBondOptions_fwd_hh
#define INCLUDED_core_scoring_hbonds_HBondOptions_fwd_hh

#include <utility/pointer/owning_ptr.hh>

namespace core{
namespace scoring{
namespace hbonds{

class HBondOptions;

typedef utility::pointer::owning_ptr< HBondOptions > HBondOptionsOP;
typedef utility::pointer::owning_ptr< HBondOptions const > HBondOptionsCOP;

} //hbonds
} //scoring
} //core

#endif // INCLUDED_core_scoring_hbonds_HBondOptions_FWD_HH
