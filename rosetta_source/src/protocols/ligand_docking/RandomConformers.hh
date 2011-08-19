// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/pack/task/ResfileReader.hh
/// @brief  this class chooses a random conformer for every residue in a specified chain
/// @author Gordon Lemmon

#ifndef INCLUDED_protocols_ligand_docking_RandomConformers_hh
#define INCLUDED_protocols_ligand_docking_RandomConformers_hh

//Auto Headers
#include <core/scoring/constraints/Constraint.hh>
#include <protocols/ligand_docking/ResidueTorsionRestraints.fwd.hh>
#include <protocols/moves/Mover.hh>
#include <set>

//// Scripter Headers
#include <utility/tag/Tag.fwd.hh>
#include <protocols/moves/DataMap.fwd.hh>
#include <protocols/filters/Filter.fwd.hh>

///////////////////////////////////////////////////////////////////////

namespace protocols {
namespace ligand_docking {

class RandomConformers : public protocols::moves::Mover
{
public:
	RandomConformers();
	virtual ~RandomConformers();
	RandomConformers(RandomConformers const & that);

	virtual protocols::moves::MoverOP clone() const;
	virtual protocols::moves::MoverOP fresh_instance() const;
	virtual std::string get_name() const;

	//void set_chain(std::string chain);
	void parse_my_tag(
		utility::tag::TagPtr const tag,
		protocols::moves::DataMap &,
		protocols::filters::Filters_map const &,
		protocols::moves::Movers_map const &,
		core::pose::Pose const &
	);

	void apply(core::pose::Pose & pose);

	// Undefined, commenting out to make PyRosetta compile
	//RandomConformers(
	//	core::pose::Pose & pose,
	//	std::set< protocols::ligand_docking::ResidueTorsionRestraintsOP > const & restraints
	//);

private:
	std::string chain_;
	void apply_residue( core::Size const residue_id, core::pose::Pose & pose );
};

} //namespace ligand_docking
} //namespace protocols

#endif
