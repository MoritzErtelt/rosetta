// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/protocols/comparative_modeling/util.hh
/// @brief small bundle of utilities for comparative modeling
/// @author James Thompson

#ifndef INCLUDED_protocols_comparative_modeling_util_hh
#define INCLUDED_protocols_comparative_modeling_util_hh

#include <core/types.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/sequence/SequenceAlignment.fwd.hh>

#include <protocols/comparative_modeling/AlignmentSet.fwd.hh>

//Auto Headers
#include <core/id/AtomID_Mask.fwd.hh>
#include <core/id/NamedAtomID.fwd.hh>
#include <protocols/loops/Loops.fwd.hh>
#include <utility/vector1.fwd.hh>

#include <map>

#ifdef WIN32
#include <string>
#endif


namespace protocols {
namespace comparative_modeling {

protocols::loops::Loops loops_from_alignment(
  core::Size nres,
	core::sequence::SequenceAlignment const & aln,
	core::Size const min_loop_size
);

protocols::loops::Loops loops_from_transitive_alignments(
	core::Size nres1,
	core::sequence::SequenceAlignment const & aln1,
	core::Size nres2,
	core::sequence::SequenceAlignment const & aln2,
	core::Size const min_loop_size
);

protocols::loops::Loops pick_loops_unaligned(
  core::Size nres,
	utility::vector1< core::Size > const & unaligned_residues,
	core::Size min_loop_size
);

protocols::loops::Loops pick_loops_chainbreak(
	core::pose::Pose & query_pose,
	core::Size min_loop_size
);

void rebuild_loops_until_closed(
	core::pose::Pose & query_pose,
	core::Size const min_loop_size,
	core::Size const max_times,
	std::string const & loop_mover_name
);

/// @brief Function for stealing ligands from a given Pose
/// and adding them to source_pose.
/// @detailed: The parameters are: @arg dest_pose Pose to which
/// ligands are added.
/// @arg source_pose_in Pose from which ligands and orientations
/// are taken.
/// @arg anchor_atom_dest anchor atom providing reference in
/// dest_pose for ligand orientation.
/// @arg anchor_atom_source anchor atom providing reference in
/// source_pose for ligand orientation.
/// @arg ligand_indices list of AtomID entries for ligand, one per
/// ligand. Ligand orientation is determined by orientation between
/// anchor_atom_source and each ligand AtomID.
void steal_ligands(
	core::pose::Pose & dest_pose,
	core::pose::Pose const & source_pose_in,
	core::id::NamedAtomID const anchor_atom_dest,
	core::id::NamedAtomID const anchor_atom_source,
	utility::vector1< core::id::NamedAtomID > const ligand_indices
);

void initialize_ss( core::pose::Pose & pose );

utility::vector1< core::pose::Pose >
templates_from_cmd_line();

bool loops_are_closed( core::pose::Pose & pose );

std::map< std::string, core::pose::Pose >
poses_from_cmd_line(
	utility::vector1< std::string > const & fn_list
);

AlignmentSet
alignments_from_cmd_line();

void randomize_selected_atoms(
	core::pose::Pose & query_pose,
	core::id::AtomID_Mask const & selected
);


} // comparative_modeling
} // protocols

#endif
