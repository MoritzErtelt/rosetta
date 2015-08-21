// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file
/// @brief

#ifndef INCLUDED_core_scoring_constraints_NamedAngleConstraint_hh
#define INCLUDED_core_scoring_constraints_NamedAngleConstraint_hh

//Unit headers
#include <core/scoring/constraints/NamedAngleConstraint.fwd.hh>
#include <core/scoring/constraints/AngleConstraint.hh>

//Core headers
#include <core/scoring/constraints/Constraint.hh>
#include <core/scoring/func/Func.hh>
#include <core/scoring/ScoreType.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/id/AtomID.hh>
#include <core/id/NamedAtomID.hh>

//Utility headers
#include <utility/vector1.hh>

// C++ Headers

namespace core {
namespace scoring {
namespace constraints {

class NamedAngleConstraint : public AngleConstraint {
public:
	NamedAngleConstraint(
		id::NamedAtomID const & a1,
		id::NamedAtomID const & a2,
		id::NamedAtomID const & a3,
		func::FuncOP func,
		ScoreType scoretype = angle_constraint );

	virtual std::string type() const;

	virtual ConstraintOP clone() const;

	/// @brief Copies the data from this Constraint into a new object and returns an OP
	/// atoms are mapped to atoms with the same name in dest pose ( e.g. for switch from centroid to fullatom )
	/// if a sequence_mapping is present it is used to map residue numbers .. NULL = identity mapping
	/// to the new object. Intended to be implemented by derived classes.
	virtual ConstraintOP remapped_clone(
		pose::Pose const & src,
		pose::Pose const & dest,
		id::SequenceMappingCOP map = NULL ) const;

	virtual void show_def( std::ostream & out, pose::Pose const & pose ) const;
	void show_def_nopose( std::ostream & out ) const;

	virtual void read_def(
		std::istream & in,
		pose::Pose const & pose,
		func::FuncFactory const & func_factory );

	virtual void score( func::XYZ_Func const & xyz, EnergyMap const &, EnergyMap & emap ) const;

private:
	id::NamedAtomID named_atom1_;
	id::NamedAtomID named_atom2_;
	id::NamedAtomID named_atom3_;
};


}
}
}

#endif
