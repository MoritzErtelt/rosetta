// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/kinematics/ShortestPathInFoldTree.fwd.hh
/// @brief  kinematics::ShortestPathInFoldTree forward declarations header
/// @author Oliver Lange


#ifndef INCLUDED_protocols_topology_broker_DofClaim_fwd_hh
#define INCLUDED_protocols_topology_broker_DofClaim_fwd_hh


// Utility headers
#include <utility/pointer/access_ptr.hh>
#include <utility/pointer/owning_ptr.hh>
// AUTO-REMOVED #include <utility/vector1.hh>

//Auto Headers
#include <utility/vector1.fwd.hh>


namespace protocols {
namespace topology_broker {

// Forward
class DofClaim;

// Types
typedef  utility::pointer::owning_ptr< DofClaim >  DofClaimOP;
typedef  utility::pointer::owning_ptr< DofClaim const >  DofClaimCOP;

typedef  utility::pointer::access_ptr< DofClaim >  DofClaimAP;
typedef  utility::pointer::access_ptr< DofClaim const >  DofClaimCAP;

typedef utility::vector1< DofClaimOP > DofClaims;


class SequenceClaim;

// Types
typedef  utility::pointer::owning_ptr< SequenceClaim >  SequenceClaimOP;
typedef  utility::pointer::owning_ptr< SequenceClaim const >  SequenceClaimCOP;

typedef  utility::pointer::access_ptr< SequenceClaim >  SequenceClaimAP;
typedef  utility::pointer::access_ptr< SequenceClaim const >  SequenceClaimCAP;

typedef utility::vector1< SequenceClaimOP > SequenceClaims;

class JumpClaim;

// Types
typedef  utility::pointer::owning_ptr< JumpClaim >  JumpClaimOP;
typedef  utility::pointer::owning_ptr< JumpClaim const >  JumpClaimCOP;

typedef  utility::pointer::access_ptr< JumpClaim >  JumpClaimAP;
typedef  utility::pointer::access_ptr< JumpClaim const >  JumpClaimCAP;

class BBClaim;

// Types
typedef  utility::pointer::owning_ptr< BBClaim >  BBClaimOP;
typedef  utility::pointer::owning_ptr< BBClaim const >  BBClaimCOP;

typedef  utility::pointer::access_ptr< BBClaim >  BBClaimAP;
typedef  utility::pointer::access_ptr< BBClaim const >  BBClaimCAP;


} // namespace kinematics
} // namespace core

#endif
