// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file devel/metal_interface/FindClosestAtom.hh
/// @brief This header contains helper functions common to metal interface
/// @author Bryan Der

#ifndef INCLUDED_devel_metal_interface_FindClosestAtom_HH
#define INCLUDED_devel_metal_interface_FindClosestAtom_HH

#include <core/conformation/Residue.hh>
#include <numeric/xyzVector.hh>

namespace devel{
namespace metal_interface{

//helper function - iterates over all sidechain non-carbon heavy atoms of res to find the one closest to xyz
std::string find_closest_atom( core::conformation::Residue const & res, core::Vector const & xyz );

}//namespace metal_interface
}//namespace devel

#endif // INCLUDED_devel_metal_interface_FindClosestAtom_HH
