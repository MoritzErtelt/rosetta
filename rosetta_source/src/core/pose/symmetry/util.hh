// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file  core/pose/symmetry/util.hh
/// @brief utility functions for handling with symmetric conformations
/// @author Ingemar Andre

#ifndef INCLUDED_core_pose_symmetry_util_hh
#define INCLUDED_core_pose_symmetry_util_hh


// Unit headers
#include <core/conformation/symmetry/SymmetricConformation.fwd.hh>
#include <core/conformation/Conformation.fwd.hh>
#include <core/scoring/Energies.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/conformation/symmetry/SymmData.fwd.hh>
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>
#include <core/kinematics/MoveMap.fwd.hh>
#include <core/kinematics/FoldTree.fwd.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/pose/PDBInfo.fwd.hh>
#include <core/types.hh>

// AUTO-REMOVED #include <utility/vector1.hh>

//Auto Headers
#include <utility/vector1.fwd.hh>

#include <string>

namespace core {
namespace pose {
namespace symmetry {

bool
is_symmetric( pose::Pose const & pose );

bool
is_symmetric( scoring::ScoreFunction const & scorefxn );

bool
is_symmetric( scoring::Energies const & energies );

conformation::symmetry::SymmetryInfoCOP
symmetry_info( pose::Pose const & pose );

void
make_symmetric_pose(
	pose::Pose & pose,
	conformation::symmetry::SymmetryInfo symmetry_info
);

void
make_symmetric_pose(
  pose::Pose & pose,
  conformation::symmetry::SymmData & symmdata
);

void
make_symmetric_pose(
  pose::Pose & pose,
	std::string symmdef_file=""
);

void
make_asymmetric_pose(
  pose::Pose & pose
);

///@brief extract the asu from a pose... unlike previous function symmetric clones are thrown out
void
extract_asymmetric_unit(core::pose::Pose const& pose_in, core::pose::Pose & pose_out);


core::pose::Pose
get_asymmetric_pose_copy_from_symmetric_pose(
  pose::Pose const & pose
);

// @details make symmetric PDBIinfo
void
make_symmetric_pdb_info(
	pose::Pose const & pose,
	pose::PDBInfoOP pdb_info_src,
	pose::PDBInfoOP pdb_info_target
);

// @details extract the pdbInfo from the asymmetric unit
void
extract_asymmetric_unit_pdb_info(
	pose::Pose const & pose,
	pose::PDBInfoCOP pdb_info_src,
	pose::PDBInfoOP pdb_info_target
);

void
make_symmetric_movemap(
  pose::Pose const & pose,
  kinematics::MoveMap & movemap
);

int
find_symmetric_basejump_anchor( pose::Pose & pose );

int
find_symmetric_basejump_anchor( pose::Pose & pose );

void
find_new_symmetric_jump_residues( core::pose::Pose & pose );

void
rotate_anchor_to_x_axis( core::pose::Pose & pose );

// find symm axis
numeric::xyzVector< core::Real >
get_symm_axis( core::pose::Pose & pose );

// a couple functions to transform between symmetric and asymmetric foldtrees
// these do not require the symm data
void
symmetrize_fold_tree( core::pose::Pose const &p, kinematics::FoldTree &f );

void
set_asymm_unit_fold_tree( core::pose::Pose &p, kinematics::FoldTree const &f);

// make a residue mask (like that used to restrict residues to repack) symmetric
void
make_residue_mask_symmetric( core::pose::Pose const &p, utility::vector1< bool > & msk );

core::kinematics::FoldTree
sealed_symmetric_fold_tree( core::pose::Pose  & pose );

} // symmetry
} // pose
} // core



#endif
