// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/core/scoring/constraints/FadeFunc.hh
/// @brief Definition for functions used in definition of constraints.
/// @author Rhiju Das

#ifndef INCLUDED_core_scoring_constraints_FadeFunc_hh
#define INCLUDED_core_scoring_constraints_FadeFunc_hh

#include <core/scoring/constraints/FadeFunc.fwd.hh>

#include <core/scoring/constraints/Func.hh>

#include <core/types.hh>

#include <utility/pointer/ReferenceCount.hh>

#include <ObjexxFCL/format.hh>
#include <basic/Tracer.hh>

// C++ Headers

namespace core {
namespace scoring {
namespace constraints {

class FadeFunc : public Func {
public:
	FadeFunc( Real const cutoff_lower, Real const cutoff_upper,
						Real const fade_zone, Real const well_depth ):
		cutoff_lower_( cutoff_lower ),
		cutoff_upper_( cutoff_upper ),
		fade_zone_( fade_zone ),
		well_depth_( well_depth ){}

	FadeFunc( Real const cutoff_lower, Real const cutoff_upper,
						Real const fade_zone ):
		cutoff_lower_( cutoff_lower ),
		cutoff_upper_( cutoff_upper ),
		fade_zone_( fade_zone ),
		well_depth_( 1.0 ){}

	FuncOP
	clone() const { return new FadeFunc( *this ); }

	Real func( Real const x ) const;
	Real dfunc( Real const x ) const;

	void read_data( std::istream& in );

	void show_definition( std::ostream &out ) const;

	Real cutoff_upper() const { return cutoff_upper_; }
	Real cutoff_lower() const { return cutoff_lower_; }
	Real fade_zone() const { return fade_zone_; }

	void cutoff_upper( Real x ) { cutoff_upper_ = x; }
	void cutoff_lower( Real x ) { cutoff_lower_ = x; }
	void fade_zone( Real x ) { fade_zone_ = x; }

	Size
	show_violations( std::ostream& out, Real x, Size verbose_level, core::Real threshold = 1 ) const;

private:
	Real cutoff_lower_;
	Real cutoff_upper_;
	Real fade_zone_;
	Real well_depth_;
};



} // constraints
} // scoring
} // core

#endif
