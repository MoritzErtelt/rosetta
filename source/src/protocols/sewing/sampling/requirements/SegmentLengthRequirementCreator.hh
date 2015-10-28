// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet;
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file SegmentLengthRequirementCreator.hh
///
/// @brief
/// @author Tim Jacobs

#ifndef INCLUDED_protocols_sewing_sampling_requirements_SegmentLengthRequirementCreator_hh
#define INCLUDED_protocols_sewing_sampling_requirements_SegmentLengthRequirementCreator_hh

//Unit headers
#include <protocols/sewing/sampling/requirements/RequirementCreator.hh>
#include <protocols/sewing/sampling/requirements/IntraSegmentRequirement.fwd.hh>

//Utility headers
#include <string>

namespace protocols {
namespace sewing  {
namespace sampling {
namespace requirements {

class SegmentLengthRequirementCreator : public IntraSegmentRequirementCreator {
	virtual IntraSegmentRequirementOP create_requirement() const;
	virtual std::string type_name() const;
};


} //requirements namespace
} //sampling namespace
} //sewing namespace
} //protocols namespace

#endif
