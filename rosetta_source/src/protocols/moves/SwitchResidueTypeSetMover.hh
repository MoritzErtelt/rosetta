// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file SwitchResidueTypeSetMover.hh
/// @brief switch between residue type sets (e.g. centroid and all atom)

#ifndef INCLUDED_protocols_moves_SwitchResidueTypeSetMover_hh
#define INCLUDED_protocols_moves_SwitchResidueTypeSetMover_hh

#include <protocols/moves/SwitchResidueTypeSetMover.fwd.hh>
#include <protocols/moves/Mover.hh>

#include <protocols/moves/DataMap.fwd.hh>
#include <protocols/filters/Filter.fwd.hh>
#include <core/pose/Pose.fwd.hh>

// C++ Headers
#include <string>

namespace protocols {
namespace moves {

/// @brief A mover that switches a pose between residue type sets (e.g. centroid and fullatom)
///
/// examples:
///     switch = SwitchResidueTypeSetMover("centroid")
/// See also:
///     Pose
///     Residue
///     ResidueType
///     ResidueTypeSet
class SwitchResidueTypeSetMover : public Mover {
public:
	SwitchResidueTypeSetMover();
	SwitchResidueTypeSetMover( std::string const & );

	/// @brief Applies ResidueTypeSet converion on the pose
	/// @note: a single Mover only converts in ONE direction e.g. to centroid
	virtual void apply( Pose & pose );
	virtual std::string get_name() const;

	void type_set_tag( std::string const & type_set_tag_in ) { type_set_tag_ = type_set_tag_in; }

	virtual MoverOP clone() const;
	virtual MoverOP fresh_instance() const;

	virtual void parse_my_tag(
		TagPtr const tag,
		DataMap & data,
		Filters_map const & filters,
		Movers_map const & movers,
		Pose const & pose );

private:
	std::string type_set_tag_;
};

} // moves
} // protocols

#endif
