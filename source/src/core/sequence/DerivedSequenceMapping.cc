// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @brief
/// @author

#include <core/id/SequenceMapping.hh>
#include <core/sequence/DerivedSequenceMapping.hh>


// ObjexxFCL headers


//Auto Headers
#include <core/kinematics/Jump.hh>


namespace core {
namespace sequence {

using namespace ObjexxFCL;

DerivedSequenceMapping::~DerivedSequenceMapping() = default;

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

bool DerivedSequenceMapping::operator == ( SequenceMapping const & rhs ) const
{
	if ( ! SequenceMapping::operator == (rhs) ) return false;

	// the parent class will have ensured that both this and rhs are of type DerivedSequenceMapping,
	// so this static cast is safe.
	auto const & rhs_downcast( static_cast< DerivedSequenceMapping const & > ( rhs ) );
	if ( seq1_       != rhs_downcast.seq1_       ) return false;
	if ( seq2_       != rhs_downcast.seq2_       ) return false;
	if ( start_seq2_ != rhs_downcast.start_seq2_ ) return false;

	return true;
}

bool DerivedSequenceMapping::same_type_as_me( SequenceMapping const & other ) const
{
	return dynamic_cast< DerivedSequenceMapping const *  > (&other);
}


} // sequence
} // core

