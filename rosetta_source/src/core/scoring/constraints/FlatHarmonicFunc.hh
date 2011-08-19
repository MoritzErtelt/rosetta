// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/core/scoring/constraints/FlatHarmonicFunc.hh
/// @brief Definition for functions used in definition of constraints.
/// @author Chris King modified from James Thompson

#ifndef INCLUDED_core_scoring_constraints_FlatHarmonicFunc_hh
#define INCLUDED_core_scoring_constraints_FlatHarmonicFunc_hh

#include <core/scoring/constraints/FlatHarmonicFunc.fwd.hh>
#include <core/scoring/constraints/Func.hh>
#include <core/types.hh>

// C++ Headers

namespace core {
namespace scoring {
namespace constraints {

class FlatHarmonicFunc : public Func {
public:
	FlatHarmonicFunc( Real const x0_in, Real const sd_in, Real const tol_in ): x0_( x0_in ), sd_( sd_in ), tol_( tol_in ){}

	FuncOP
	clone() const { return new FlatHarmonicFunc( *this ); }

	Real func( Real const x ) const;
	Real dfunc( Real const x ) const;

	void read_data( std::istream & in );

	void show_definition( std::ostream &out ) const;

	Real x0() const {
		return x0_;
	}

	Real sd() const {
		return sd_;
	}

	Real tol() const {
		return tol_;
	}

	void x0( Real x ) {
		x0_ = x;
	}

	void sd( Real sd ) {
		sd_ = sd;
	}

	void tol( Real tol ) {
		tol_ = tol;
	}

	Size
	show_violations( std::ostream& out, Real x, Size verbose_level, core::Real threshold = 1 ) const;

private:
	Real x0_;
	Real sd_;
	Real tol_;
};

} // constraints
} // scoring
} // core

#endif
