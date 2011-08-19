// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/match/SixDHasher.fwd.hh
/// @brief  Forward declaration for classes in 6D hasher
/// @author Alex Zanghellini (zanghell@u.washington.edu)
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com), porting to mini

#ifndef INCLUDED_protocols_match_SixDHasher_fwd_hh
#define INCLUDED_protocols_match_SixDHasher_fwd_hh

// Package headers
// AUTO-REMOVED #include <protocols/match/Hit.hh>

// Project headers
#include <core/types.hh>

// Utility headers
#include <utility/pointer/owning_ptr.hh>
// AUTO-REMOVED #include <utility/fixedsizearray1.hh>


// Boost headers
// AUTO-REMOVED #include <boost/unordered/unordered_map_fwd.hpp>

//Auto Headers
#include <utility/fixedsizearray1.fwd.hh>
#include <iterator>


namespace protocols {
namespace match {

//typedef utility::fixedsizearray1< core::Size, 3 > Size3; -- in Hit.fwd.hh
//typedef utility::fixedsizearray1< core::Real, 6 > Real6; -- in Hit.fwd.hh

typedef utility::fixedsizearray1< core::Size, 6 > Size6;
typedef utility::fixedsizearray1< core::Size, 3 > Bin3D;
typedef utility::fixedsizearray1< core::Real, 3 > Real3;
typedef utility::fixedsizearray1< core::Size, 6 > Bin6D;

typedef Bin3D xyzbin;
typedef Bin3D eulerbin;

struct Bin3D_equals;
struct Bin3D_hash;

typedef Bin3D         center_of_mass_binned;
typedef Bin3D         euler_angles_binned;
typedef Bin3D_equals  euler_equals;
typedef Bin3D_hash    euler_hash;
typedef Bin3D_equals  xyzbin_hash;
typedef Bin3D_hash    xyzbin_equals;


//typedef boost::unordered_map< euler_angles_binned, MatchSetOP, euler_hash, euler_equals > EulerHash;
//typedef utility::pointer::owning_ptr< EulerHash > EulerHashOP;

//typedef boost::unordered_map< center_of_mass_binned, EulerHashOP, xyzbin_hash, xyzbin_equals > SixDHash
//typedef utility::pointer::owning_ptr< SixDHash > SixDHashOP;

struct bin_index_hasher;

class SixDCoordinateBinner;
typedef utility::pointer::owning_ptr< SixDCoordinateBinner > SixDCoordinateBinnerOP;
typedef utility::pointer::owning_ptr< SixDCoordinateBinner const > SixDCoordinateBinnerCOP;



}
}

#endif
