// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/pack/scmin/SCMinMinimizerMap.hh
/// @brief  Class for identifying the sidechain DOFs in the AtomTree which are free during
///         any particular call to the minimizer.
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

#ifndef INCLUDED_core_pack_scmin_SCMinMinimizerMap_hh
#define INCLUDED_core_pack_scmin_SCMinMinimizerMap_hh

// Unit headers
#include <core/pack/scmin/SCMinMinimizerMap.fwd.hh>

// Package Headers
#include <core/pack/scmin/AtomTreeCollection.fwd.hh>

// Project Headers
#include <core/types.hh>
#include <core/conformation/Residue.fwd.hh>
#include <core/id/AtomID_Map.hh>
#include <core/id/DOF_ID.hh>
#include <core/id/DOF_ID_Mask.hh>
#include <core/id/TorsionID.hh>
#include <core/kinematics/MinimizerMapBase.hh>
#include <core/kinematics/AtomTree.fwd.hh>
#include <core/kinematics/tree/Atom.fwd.hh>
#include <core/kinematics/DomainMap.hh>
#include <core/scoring/DerivVectorPair.hh>
#include <core/optimization/types.hh>
#include <core/optimization/DOF_Node.fwd.hh>

//#include <core/pose/Pose.fwd.hh>
//#include <core/scoring/ScoreFunction.fwd.hh>

// Utility headers
#include <utility/vector1.hh>
#include <utility/pointer/ReferenceCount.hh>

namespace core {
namespace pack {
namespace scmin {

/// @brief
class SCMinMinimizerMap : public kinematics::MinimizerMapBase
{
public:
	typedef optimization::DOF_Node   DOF_Node;
	typedef optimization::DOF_NodeOP DOF_NodeOP;

public:
	SCMinMinimizerMap();
	virtual ~SCMinMinimizerMap();

	/// @brief the SCMinMinimizerMap has to know how many residues are in the pose; this allows
	/// it to do O(1) updates to its DomainMap -- this function costs O(N).
	void set_total_residue( Size total_residue );

	/// @brief Disable the minimization for all residues.  Ammortized O(1).
	void clear_active_chi();

	/// @brief Activate all the chi for a particular residue.  Ammortized O(1).
	void activate_residue_chi( Size resindex );

	/// @brief Invoked during the depth-first traversal through the AtomTree.  The AtomTree
	/// is indicating that a particular torsion is dependent on another torsion.  Record
	/// that fact.
	virtual
	void
	add_torsion(
		DOF_ID const & new_torsion,
		DOF_ID const & parent
	);

	/// @brief Invoked during the depth-first traversal through the AtomTree; the atom
	/// tree is indicating that a given atom is controlled by a particular DOF.  Record
	/// that fact.
	virtual
	void
	add_atom(
		AtomID const & atom_id,
		DOF_ID const & dof_id
	);

	/// @brief Traverse the atom trees in preparation for minimization to tie together all the
	/// DOFs and the atoms they control.
	void
	setup( AtomTreeCollectionOP trees );

public:

	/// Accessors
	Size nactive_residues() const { return nactive_residues_; }
	Size active_residue( Size index ) const { assert( index <= nactive_residues_ ); return active_residues_[ index ]; }

	/// @brief MinimizerMapBase class virtual accessor
	virtual kinematics::DomainMap const & domain_map() const { return domain_map_; }

	/// @brief Inline accessor
	inline kinematics::DomainMap const & dm() const { return domain_map_; }

	/// @brief Convenience lookup -- turns over the request to the AtomTreeCollection
	conformation::Residue const &
	residue( Size seqpos ) const;

	Size n_dof_nodes() const { return n_active_dof_nodes_; }

	/// @brief Initialize a multivec with the chi reflected in the current residue(s)
	void starting_dofs( optimization::Multivec & chi ) const;
	/// @brief Assign the chi values to the residue(s)
	void assign_dofs_to_mobile_residues( optimization::Multivec const & chi );

	optimization::DOF_Node &
	dof_node( Size index ) {
		assert( index > 0 && index <= n_active_dof_nodes_ );
		return *dof_nodes_[ index ];
	}

	optimization::DOF_Node const &
	dof_node_for_chi( Size resid, Size chiid ) const;

	id::TorsionID
	tor_for_dof( id::DOF_ID const & dofid ) const;

	kinematics::tree::Atom const &
	atom( AtomID const & atid ) const;

	void zero_atom_derivative_vectors();

	/// @brief propagate f1/f2's up from children to parents
	void link_torsion_vectors();

	void set_natoms_for_residue( Size resid, Size natoms );

	utility::vector1< scoring::DerivVectorPair > &
	atom_derivatives( Size resid ) {
		return atom_derivatives_[ resid ];
	}

private:
	void reset_dof_nodes();

	optimization::DOF_NodeOP dof_nodeop( id::DOF_ID );

private:

	Size nactive_residues_;
	utility::vector1< Size > active_residues_;
	utility::vector1< Size > active_residue_index_for_res_;
	utility::vector1< Size > chi_start_for_active_residue_; // what's the index of the first chi for a particular active residue?
	utility::vector1< utility::vector1< Size > > active_residue_atom_to_dofnode_index_;

	utility::vector1< utility::vector1< scoring::DerivVectorPair > > atom_derivatives_;

	kinematics::DomainMap    domain_map_;
	utility::vector1< ResidueAtomTreeCollectionOP > atcs_for_residues_;

	id::DOF_ID_Mask dof_mask_; // used in recursive descent through an atom tree
	utility::vector1< optimization::DOF_NodeOP > dof_nodes_;

	AtomTreeCollectionOP atom_tree_collection_;

	/// each atom tree in the AtomTreeCollection will tell us that it represents residue 1.
	/// this variable tells us which residue is actually being represented.
	Size focused_residue_;

	/// For parent_dof lookup: track which chi a particular atom is responsible for.
	Size chi_start_for_focused_residue_;
	Size nchi_added_for_focused_residue_;
	utility::vector1< Size > atoms_representing_chis_;

	/// Don't destory DOF_Nodes between rounds of minimization, just mark them as inactive.
	Size n_active_dof_nodes_;

};



} // namespace scmin
} // namespace pack
} // namespace core

#endif
