// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet;
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file Model.fwd.hh
///
/// @brief
/// @author Tim Jacobs



#ifndef INCLUDED_protocols_sewing_conformation_Model_FWD_HH
#define INCLUDED_protocols_sewing_conformation_Model_FWD_HH

#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace sewing  {

struct Model;
typedef utility::pointer::shared_ptr< Model > ModelOP;
typedef utility::pointer::shared_ptr< Model const > ModelCOP;

struct SewSegment;
typedef utility::pointer::shared_ptr< SewSegment > SewSegmentOP;
typedef utility::pointer::shared_ptr< SewSegment const > SewSegmentCOP;

} //sewing namespace
} //protocols namespace

#endif


