// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/flxbb/DesignTask.fwd.hh
/// @brief
/// @author Nobuyasu Koga (nobuyasu@uw.edu)
///

#ifndef INCLUDED_protocols_flxbb_DesignTask_fwd_hh
#define INCLUDED_protocols_flxbb_DesignTask_fwd_hh

#include <utility/pointer/owning_ptr.hh>
// AUTO-REMOVED #include <utility/vector1.hh>

//Auto Headers
#include <utility/vector1.fwd.hh>


namespace protocols {
namespace flxbb {

///////////////////////////////////////////////////////////////////////////////////////////////////////
class DesignTask;
typedef utility::pointer::owning_ptr< DesignTask > DesignTaskOP;
typedef utility::vector1< DesignTaskOP > DesignTaskSet;
typedef utility::pointer::owning_ptr< DesignTaskSet > DesignTaskSetOP;

///////////////////////////////////////////////////////////////////////////////////////////////////////
class DesignTask_Normal;
typedef utility::pointer::owning_ptr< DesignTask_Normal > DesignTask_NormalOP;

///////////////////////////////////////////////////////////////////////////////////////////////////////
class DesignTask_Layer;
typedef utility::pointer::owning_ptr< DesignTask_Layer > DesignTask_LayerOP;


} // namespace flxbb
} // namespace protocols

#endif
