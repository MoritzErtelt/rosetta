// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 sw=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file core/util/SwitchResidueTypeSet.hh
/// @brief Functions for switching the residue type set of a pose
/// @author P. Douglas Renfrew (renfrew@nyu.edu)

#ifndef INCLUDED_core_util_SwitchResidueTypeSet_hh
#define INCLUDED_core_util_SwitchResidueTypeSet_hh

// Unit headers
#include <core/pose/Pose.hh>

namespace core {
namespace util {

/// @details the function allows a pose to use a different residue_type_set to
/// represent all its residues, such as from fullatom residues to centroid
/// residues, or vice versa. During the switch, corresponding atoms will be
/// copied. Redundant atoms will be removed (in case from fullatom to centroid)
/// and missing atoms will be built by ideal geometry (in the case from centroid
/// to fullatom).
void
switch_to_residue_type_set(
	core::pose::Pose & pose,
	std::string const & type_set_name,
	bool allow_sloppy_match = false
	);

} // util
} // core

#endif //INCLUDED_core_util_switchresiduetypeset_HH

