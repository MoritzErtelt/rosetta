// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/methods/EtableEnergy.hh
/// @brief  Etable energy method class declaration
/// @author Phil Bradley
/// @author Andrew Leaver-Fay
/// @author Christopher Miles (cmiles@uw.edu)

#ifndef INCLUDED_core_scoring_methods_LinearChainbreakEnergy_hh
#define INCLUDED_core_scoring_methods_LinearChainbreakEnergy_hh

// Unit headers
#include <core/scoring/methods/LinearChainbreakEnergy.fwd.hh>

// Package headers
#include <core/types.hh>
#include <core/kinematics/ShortestPathInFoldTree.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/scoring/EnergyMap.fwd.hh>
#include <core/scoring/methods/WholeStructureEnergy.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

// Third-party headers
#include <boost/scoped_ptr.hpp>

// C++ headers
#include <cstdlib>

using boost::scoped_ptr;
using core::Size;
using core::kinematics::ShortestPathInFoldTree;

namespace core {
namespace scoring {
namespace methods {

/// @brief LinearChainbreakEnergy class iterates across all residues in finalize()
/// and determines the penalty between residues i and i+1 by how much their
/// psueduo atoms do not align.
class LinearChainbreakEnergy : public WholeStructureEnergy {
public:
	typedef WholeStructureEnergy parent;

	// @brief Creates a new LinearChainbreakEnergy with the default allowable sequence
	// separation (+inf)
	LinearChainbreakEnergy();

	// @brief Create a new LinearChainbreakEnergy with the specified allowable sequence
	// separation
	explicit LinearChainbreakEnergy(Size allowable_sequence_sep);

	/// @brief The auto-generated copy constructor does not properly handle smart
	/// pointer types, so we must explicitly define our own.
	LinearChainbreakEnergy(const LinearChainbreakEnergy&);

	/// @brief The auto-generated operator=() method does not properly handle pointer types.
	LinearChainbreakEnergy& operator=(const LinearChainbreakEnergy&);

	// @brief Releases resources associated with an instance.
	~LinearChainbreakEnergy();

	/// clone
	virtual EnergyMethodOP clone() const {
		return new LinearChainbreakEnergy(*this);
	}

	/// called at the end of energy evaluation
	virtual void finalize_total_energy(
		pose::Pose & pose,
		ScoreFunction const &,
		EnergyMap & totals) const;

	/// called during gradient-based minimization inside dfunc
	/**
	F1 and F2 are not zeroed -- contributions from this atom are
	just summed in
	**/
	virtual void eval_atom_derivative(id::AtomID const & id,
		pose::Pose const & pose,
		kinematics::DomainMap const & domain_map,
		ScoreFunction const & sfxn,
		EnergyMap const & weights,
		Vector & F1,
		Vector & F2) const;

	virtual void indicate_required_context_graphs( utility::vector1< bool > & ) const;

private:
	// Initialization routine common to both constructor
	void initialize(Size allowable_sequence_sep);

	// Maximum allowable sequence separation permitted for scoring
	Size allowable_sequence_sep_;

	// Every call to finalize_total_energy() requires us to compute shortest
	// paths between the residues defining a cutpoint. The
	// ShortestPathInFoldTree object computes pair-wise shortest paths
	// between all residues in its constructor. However, it is unaware of
	// changes to the FoldTree used to initialize it. As a result, we can
	// only reuse a ShortestPathInFoldTree object if we are certain that the
	// FoldTree has not changed.
	mutable scoped_ptr<ShortestPathInFoldTree> shortest_paths_;

	// FoldTree::hash_value() provides a time- and space-efficient mechanism
	// to compare a pair of FoldTree's. Each time we are forced to recompute
	// the ShortestPathInFoldTree (either because it has yet to be
	// initialized or because the FoldTree has changed), we take note of the
	// new FoldTree's hash value.
	mutable size_t previous_hash_value_;

	virtual
	core::Size version() const;
};

} // methods
} // scoring
} // core
#endif
