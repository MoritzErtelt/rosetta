// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file LegacyNodeConstraintAssemblyMover.hh
///
/// @brief A Mover that combines Smotif models into a continuous protein design.
/// @author Tim Jacobs


#ifdef NOT_IN_SCONS_DEPRECATED

#ifndef INCLUDED_protocols_legacy_sewing_sampling_LegacyNodeConstraintAssemblyMover_HH
#define INCLUDED_protocols_legacy_sewing_sampling_LegacyNodeConstraintAssemblyMover_HH

// Unit Headers
#include <protocols/legacy_sewing/sampling/LegacyNodeConstraintAssemblyMover.fwd.hh>
//#include <protocols/legacy_sewing/sampling/LegacyMotifDirectedAssemblyMover.hh>

// Package Headers
#include <protocols/legacy_sewing/conformation/Assembly.hh>
#include <protocols/legacy_sewing/sampling/SewGraph.fwd.hh>

#include <protocols/moves/Mover.hh>
#include <protocols/filters/Filter.hh>

#include <core/pose/Pose.fwd.hh>

// Utility Headers

namespace protocols {
namespace legacy_sewing  {

struct NodeConstraint {

	NodeConstraint():
		ideal_distance_(0.0),
		distance_range_(2.0),
		ideal_hoist_(0.0),
		hoist_range_(10.0),
		ideal_packing_(0.0),
		packing_range_(10.0),
		ideal_meridian_(0.0),
		meridian_range_(5.0)
	{}

	core::Real ideal_distance_;
	core::Real distance_range_;

	core::Real ideal_hoist_;
	core::Real hoist_range_;

	core::Real ideal_packing_;
	core::Real packing_range_;

	core::Real ideal_meridian_;
	core::Real meridian_range_;

	std::string ss1_;
	std::string ss2_;

};

	//The parent class doesn't exist!

class LegacyNodeConstraintAssemblyMover : public LegacyMotifDirectedAssemblyMover {

public:

	typedef LegacyMotifDirectedAssemblyMover parent;

	LegacyNodeConstraintAssemblyMover();

	protocols::moves::MoverOP
	clone() const;

	protocols::moves::MoverOP
	fresh_instance() const;

	std::string
	get_name() const;

	virtual
	void
	apply(
		core::pose::Pose & pose
	);

	bool
	check_all_nodes() const;

	virtual
	ModelNode const *
	get_starting_model_node();

	bool
	check_constraints(
		core::Size index,
		ModelNode const * node
	) const;

	///@brief Extend base class method to
	///with checks for constraints
	virtual
	bool
	check_edge(
		AssemblyOP assembly,
		ModelNode const * const reference_node,
		HashEdge const * const new_edge,
		bool allow_repeat = false
	);

private:

	utility::vector1<NodeConstraint> node_constraints_;

};

} //legacy_sewing
} //protocols

#endif

#endif
