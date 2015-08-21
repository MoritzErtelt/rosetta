// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/loops/loop_mover/perturb/LoopMover_QuickCCDCreator.hh
/// @brief  Header for LoopMover_QuickCCDCreator
/// @author Matthew O'Meara

#ifndef INCLUDED_protocols_loops_loop_mover_perturb_LoopMover_QuickCCDCreator_hh
#define INCLUDED_protocols_loops_loop_mover_perturb_LoopMover_QuickCCDCreator_hh

// Unit Headers
#include <protocols/moves/MoverCreator.hh>

namespace protocols {
namespace loops {
namespace loop_mover {
namespace perturb {

/// @brief creator for the LoopMover_Perturb_QuickCCDCreator class
class LoopMover_Perturb_QuickCCDCreator : public moves::MoverCreator
{
public:
	LoopMover_Perturb_QuickCCDCreator() {};
	virtual ~LoopMover_Perturb_QuickCCDCreator();

	virtual moves::MoverOP create_mover() const;
	virtual std::string keyname() const;

};

} //namespace perturb
} //namespace loop_mover
} //namespace loops
} //namespace protocols

#endif //INCLUDED_protocols_loops_loop_mover_perturb_LoopMover_QuickCCDCreator_hh
