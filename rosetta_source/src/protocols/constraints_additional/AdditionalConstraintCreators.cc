// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/protocols/constraints_additional/AdditionalConstraintCreators.hh
/// @brief  Base class for ConstraintCreators for the Constraint load-time factory registration scheme
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)


// Unit Headers
#include <protocols/constraints_additional/AdditionalConstraintCreators.hh>

/// Project Headers
#include <protocols/constraints_additional/SequenceProfileConstraint.hh>
#include <core/scoring/constraints/NamedAtomPairConstraint.hh>
#include <protocols/constraints_additional/BindingSiteConstraint.hh>
#include <protocols/constraints_additional/PocketConstraint.hh>

namespace protocols {
namespace constraints_additional {


BindingSiteConstraintCreator::BindingSiteConstraintCreator() {}
BindingSiteConstraintCreator::~BindingSiteConstraintCreator() {}

core::scoring::constraints::ConstraintOP
BindingSiteConstraintCreator::create_constraint() const {
	return new BindingSiteConstraint;
}

std::string BindingSiteConstraintCreator::keyname() const
{
	return "BindingSite";
}

SequenceProfileConstraintCreator::SequenceProfileConstraintCreator() {}
SequenceProfileConstraintCreator::~SequenceProfileConstraintCreator() {}

core::scoring::constraints::ConstraintOP
SequenceProfileConstraintCreator::create_constraint() const
{
	return new SequenceProfileConstraint;
}

std::string
SequenceProfileConstraintCreator::keyname() const
{
	return "SequenceProfile";
}

NamedAtomPairConstraintCreator::NamedAtomPairConstraintCreator() {}
NamedAtomPairConstraintCreator::~NamedAtomPairConstraintCreator() {}

core::scoring::constraints::ConstraintOP
NamedAtomPairConstraintCreator::create_constraint() const
{
	return new core::scoring::constraints::NamedAtomPairConstraint( core::id::NamedAtomID(), core::id::NamedAtomID(), NULL);
}

std::string
NamedAtomPairConstraintCreator::keyname() const {
	return "AtomPair";
}

PocketConstraintCreator::PocketConstraintCreator() {}
PocketConstraintCreator::~PocketConstraintCreator() {}

core::scoring::constraints::ConstraintOP
PocketConstraintCreator::create_constraint() const {
	return new PocketConstraint;
}

std::string PocketConstraintCreator::keyname() const
{
	return "Pocket";
}


} //namespace constraints_additional
} //namespace protocols

