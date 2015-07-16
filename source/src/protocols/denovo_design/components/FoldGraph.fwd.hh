// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   src/protocols/denovo_design/FoldGraph.fwd.hh
/// @brief  FoldGraph forward header
/// @author Tom Linsky


#ifndef INCLUDED_protocols_denovo_design_FoldGraph_fwd_hh
#define INCLUDED_protocols_denovo_design_FoldGraph_fwd_hh


// Utility headers
#include <utility/pointer/access_ptr.fwd.hh>
#include <utility/pointer/owning_ptr.fwd.hh>

// C++ headers

namespace protocols {
namespace denovo_design {
namespace components {

// Forward
class FoldGraph;

// Pointer Types
typedef utility::pointer::shared_ptr< FoldGraph > FoldGraphOP;
typedef utility::pointer::shared_ptr< FoldGraph const > FoldGraphCOP;

typedef utility::pointer::weak_ptr< FoldGraph > FoldGraphAP;
typedef utility::pointer::weak_ptr< FoldGraph const > FoldGraphCAP;

} // namespace components
} // namespace denovo_design
} // namespace protocols

#endif
