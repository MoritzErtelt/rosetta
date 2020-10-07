// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/core/pack/guidance_scoreterms/sap/SapConstraint.fwd.hh
/// @brief Apply the sap_score as a sequence constraint to the pose
/// @details
/// @author Brian Coventry (bcov@uw.edu)


#ifndef INCLUDED_core_pack_guidance_scoreterms_sap_SapConstraint_fwd_hh
#define INCLUDED_core_pack_guidance_scoreterms_sap_SapConstraint_fwd_hh

// Utility header
#include <utility/pointer/owning_ptr.hh>

namespace core {
namespace pack {
namespace guidance_scoreterms {
namespace sap {

class SapConstraint;

typedef utility::pointer::shared_ptr< SapConstraint > SapConstraintOP;
typedef utility::pointer::shared_ptr< SapConstraint const > SapConstraintCOP;

} //sap
} //guidance_scoreterms
} //pack
} //core

#endif

