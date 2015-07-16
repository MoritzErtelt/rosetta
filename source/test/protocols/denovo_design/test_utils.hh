// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file test/protocols/denovo_design/components/test_utils.hh
/// @brief Common test routines for permutation/component unit tests
/// @detailed
/// @author Tom Linsky


#ifndef INCLUDED_protocols_denovo_design_components_test_utils_hh
#define INCLUDED_protocols_denovo_design_components_test_utils_hh

// Unit headers
#include <protocols/denovo_design/components/Segment.hh>
#include <protocols/denovo_design/components/StructureData.hh>

// Protocol headers

// Package headers

// Core headers
#include <core/types.hh>

// Basic/Numeric/Utility Headers
#include <basic/Tracer.hh>

// C++ Headers

namespace protocols {
namespace denovo_design {

// void checks a component in two poses to see if relative residue positions have moved
core::Size new_res(
		core::Size const resi,
		protocols::denovo_design::components::StructureData const & orig,
		protocols::denovo_design::components::StructureData const & newp );

// checks for unwanted movement between the two given components
void check_unwanted_movement(
		std::string const & comp1,
		std::string const & comp2,
		protocols::denovo_design::components::StructureData const & orig,
		protocols::denovo_design::components::StructureData const & newp );

void check_movable_group(
		protocols::denovo_design::components::StructureData const & orig,
		protocols::denovo_design::components::StructureData const & perm,
		core::Size const group );

void check_unwanted_movement(
		protocols::denovo_design::components::StructureData const & orig,
		protocols::denovo_design::components::StructureData const & perm );

void check_sequential(
		protocols::denovo_design::components::StructureData const & perm,
		std::string const & c1,
		std::string const & c2,
		std::string const & c3 );

}
}
#endif
