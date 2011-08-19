// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/optimization/cartesian_minimize.cc
/// @brief  Atom tree minimization functions
/// @author Frank DiMaio

#ifndef INCLUDED_core_optimization_cartesian_minimize_hh
#define INCLUDED_core_optimization_cartesian_minimize_hh


// Package headers
#include <core/optimization/types.hh>
#include <core/optimization/CartesianMinimizerMap.hh>
#include <core/optimization/NumericalDerivCheckResult.fwd.hh>
#include <core/optimization/DOF_Node.fwd.hh>
#include <core/kinematics/tree/Atom.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>

//Auto Headers
#include <core/optimization/CartesianMultifunc.fwd.hh>



namespace core {
namespace optimization {


void
cartesian_dfunc(
	pose::Pose & pose,
	CartesianMinimizerMap & min_map,
	scoring::ScoreFunction const & scorefxn,
	Multivec const & vars,
	Multivec & dE_dvars
);

void cartesian_collect_torsional_deriv(
	pose::Pose & pose,
	CartesianMinimizerMap & min_map,
	core::scoring::ScoreFunction const & scorefxn,
	Multivec & dE_dvars,
	core::Real scale
);

void
cartesian_collect_atompairE_deriv(
	pose::Pose & pose,
	CartesianMinimizerMap & min_map,
	scoring::ScoreFunction const & scorefxn,
	Multivec & dE_dvars,
	core::Real scale
);


void
cart_numerical_derivative_check(
	CartesianMinimizerMap const & min_map,
	CartesianMultifunc const & func,
	Multivec const & start_vars,
	Multivec const & dE_dvars,
	NumericalDerivCheckResultOP deriv_check_result,
	bool const verbose // = true
);


} // namespace optimization
} // namespace core

#endif // INCLUDED_core_optimization_cartesian_minimize_hh
