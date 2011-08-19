// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file /protocols/moves/TaskAwareMinMover.hh
/// @brief MinMover which updates MoveMap to mirror PackerTask
/// @author Steven Lewis

#ifndef INCLUDED_protocols_moves_TaskAwareMinMover_hh
#define INCLUDED_protocols_moves_TaskAwareMinMover_hh

// Unit Headers
#include <protocols/moves/TaskAwareMinMover.fwd.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <protocols/moves/Mover.hh>
#include <protocols/moves/MinMover.fwd.hh>
#include <protocols/filters/Filter.fwd.hh>

//#include <core/pack/task/PackerTask.fwd.hh>
#include <core/pack/task/TaskFactory.fwd.hh>

// Utility Headers
// AUTO-REMOVED #include <core/types.hh>

namespace protocols {
namespace moves {

///@details this class wraps MinMover, but ensures that its MoveMap always contains up-to-date information about sidechain mobility.  It takes its base movemap, allows sidechain freedom at any position mobile in a Factory-generated PackerTask, and passes the new movemap to MinMover.  The MinMover's MoveMap does not accumulate state over many calls to apply().
class TaskAwareMinMover : public protocols::moves::Mover {

public:

	TaskAwareMinMover();

	///@brief constructor with TaskFactory
	TaskAwareMinMover(
		protocols::moves::MinMoverOP minmover_in,
		core::pack::task::TaskFactoryCOP factory_in
		);

	virtual ~TaskAwareMinMover();

	virtual void apply( core::pose::Pose & pose );
	virtual std::string get_name() const;

	virtual MoverOP fresh_instance() const;
	virtual MoverOP clone() const;

	///@brief parse XML (specifically in the context of the parser/scripting scheme)
	virtual void parse_my_tag(
		TagPtr const,
		DataMap &,
		Filters_map const &,
		Movers_map const &,
		Pose const & );

	///@brief parse "task_operations" XML option
	virtual void parse_task_operations(
		TagPtr const,
		DataMap const &,
		Filters_map const &,
		Movers_map const &,
		Pose const & );

	void chi( bool val ) { chi_ = val; }
	void bb( bool val ) { bb_ = val; }
	bool chi() const { return chi_; }
	bool bb() const { return bb_; }

private:
	///@brief OP for MinMover
	protocols::moves::MinMoverOP minmover_;
	///@brief OP for constant task factory for nonconstant tasks, if present
	core::pack::task::TaskFactoryCOP factory_;
	bool chi_, bb_;

};//end TaskAwareMinMover

}//namespace moves
}//namespace protocols

#endif // INCLUDED_protocols_moves_TaskAwareMinMover_HH
