// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/moves/protein_interface_design/movers/BuildAlaPose.fwd.hh
/// @brief  BuildAlaPose forward declarations header
/// @author Ben Stranges (stranges@unc.edu)

#ifndef INCLUDED_protocols_protein_interface_design_movers_BuildAlaPose_fwd_hh
#define INCLUDED_protocols_protein_interface_design_movers_BuildAlaPose_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace protein_interface_design {
namespace movers {

//Forwards and OP typedefs
class BuildAlaPose;
typedef utility::pointer::shared_ptr< BuildAlaPose > BuildAlaPoseOP;
typedef utility::pointer::shared_ptr< BuildAlaPose const > BuildAlaPoseCOP;

}//movers
}//protein_interface_design
}//protocols

#endif //INCLUDED_protocols_protein_interface_design_movers_BuildAlaPose_FWD_HH
