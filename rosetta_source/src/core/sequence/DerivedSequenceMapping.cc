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
/// @author

#include <core/types.hh>
#include <core/id/SequenceMapping.hh>
#include <core/sequence/DerivedSequenceMapping.hh>
#include <core/conformation/Conformation.hh>
#include <core/conformation/signals/LengthEvent.hh>
#include <basic/Tracer.hh>
#include <utility/exit.hh>
#include <core/kinematics/AtomWithDOFChange.hh>

#include <algorithm> // for max_element

// ObjexxFCL headers
#include <ObjexxFCL/string.functions.hh>

namespace core {
namespace sequence {

using namespace ObjexxFCL;

DerivedSequenceMapping::~DerivedSequenceMapping() {}

DerivedSequenceMapping::DerivedSequenceMapping( DerivedSequenceMapping const & src )
	: SequenceMapping(src)
{
	*this = src;
}

DerivedSequenceMapping &
DerivedSequenceMapping::operator = ( DerivedSequenceMapping const & src ) {
	seq1_       = src.seq1();
	seq2_       = src.seq2();
	size2(        src.size2()   );
	mapping(      src.mapping() );
	start_seq2_ = src.start_seq2();

	return *this;
}

} // sequence
} // core

