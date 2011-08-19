// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file   core/scoring/symmetry/SymmetricEnergies.hh
/// @brief  Symmetric Energies class to store cached energies and track the residue
/// neighbor relationships
/// @author Ingemar Andre

#ifndef INCLUDED_core_scoring_symmetry_SymmetricEnergies_hh
#define INCLUDED_core_scoring_symmetry_SymmetricEnergies_hh

// Unit headers
#include <core/scoring/symmetry/SymmetricEnergies.fwd.hh>

// Package headers
#include <core/scoring/Energies.hh>
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>
#include <core/conformation/symmetry/SymmetricConformation.fwd.hh>

// Project headers
#include <core/conformation/PointGraph.fwd.hh>
#include <core/scoring/ContextGraphTypes.hh>

// Numeric headers
#include <numeric/xyzMatrix.hh>

namespace core {
namespace scoring {
namespace symmetry {

class SymmetricEnergies : public Energies
{

public:

	typedef conformation::symmetry::SymmetricConformation SymmetricConformation;
	typedef conformation::symmetry::SymmetryInfoCOP SymmetryInfoCOP;

	typedef numeric::xyzMatrix< Real > Matrix;

public:
	/// ctor -- ensure correct initial state
	SymmetricEnergies();

	// Explicit copy ctor to avoid #include .hh's
	SymmetricEnergies( Energies const & src );

	SymmetricEnergies( Energies & src );

	// Explicit assignmnet operator to avoid #include .hh's
	virtual Energies const & operator = ( Energies const & rhs );

	///@details determine whether my type is the same as another Conformation's
	virtual
	bool
	same_type_as_me( Energies const & other, bool recurse = true ) const;

	~SymmetricEnergies();

	EnergiesOP
	clone() const;

	void set_derivative_graph( MinimizationGraphOP dg );
	MinimizationGraphOP derivative_graph();
	MinimizationGraphCOP derivative_graph() const;

private:

	void update_neighbor_links( pose::Pose const & pose );
	void fill_point_graph( pose::Pose const & pose, conformation::PointGraphOP pg ) const;
	void require_context_graph_( scoring::ContextGraphType type, bool external ) const;

private:
	MinimizationGraphOP derivative_graph_;

};

} // namespace symmetry
} // namespace scoring
} // namespace core


#endif // INCLUDED_core_scoring_symmetry_SymmetricEnergies_HH
