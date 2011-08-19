// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file
/// @brief  fwd headers for ns jumping
/// @author Oliver Lange


#ifndef INCLUDED_protocols_jumping_DisulfPairingsList_fwd_hh
#define INCLUDED_protocols_jumping_DisulfPairingsList_fwd_hh


// Utility headers
//#include <utility/pointer/access_ptr.fwd.hh>
// AUTO-REMOVED #include <utility/pointer/owning_ptr.fwd.hh>
// AUTO-REMOVED #include <utility/vector1.hh>

//Auto Headers
#include <utility/vector1.fwd.hh>


namespace protocols {
namespace jumping {

// Forward
//class BaseJumpSetup;
///@brief list of pairings
class DisulfPairing;
typedef utility::vector1<DisulfPairing> DisulfPairingsList;

// new better version of the name --- try to phase out DisulfPairingsList
typedef utility::vector1<DisulfPairing> DisulfPairingList;


// Types
// NO DisulfPairingsList is not a Ref-Counted class
//typedef  utility::pointer::owning_ptr< DisulfPairingsList >  DisulfPairingsListOP;
//typedef  utility::pointer::owning_ptr< DisulfPairingsList const >  DisulfPairingsListCOP;



} // namespace jumping
} // namespace protocols

#endif
