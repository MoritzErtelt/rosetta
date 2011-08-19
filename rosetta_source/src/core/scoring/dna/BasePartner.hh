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

#ifndef INCLUDED_core_scoring_dna_BasePartner_hh
#define INCLUDED_core_scoring_dna_BasePartner_hh

#include <core/scoring/dna/BasePartner.fwd.hh>

// AUTO-REMOVED #include <core/types.hh>

#include <basic/datacache/CacheableData.hh>
#include <core/pose/Pose.fwd.hh>

// AUTO-REMOVED #include <utility/vector1.hh>

//Auto Headers
#include <utility/vector1_bool.hh>


namespace core {
namespace scoring {
namespace dna {

/// silly vector1 wrapper class so we can derive from PoseCachedData

class BasePartner : public basic::datacache::CacheableData {
public:

	BasePartner( utility::vector1< Size > const & partner_in ):
		partner_( partner_in )
	{}

	basic::datacache::CacheableDataOP
	clone() const
	{
		return new BasePartner( *this );
	}

	Size
	size() const
	{
		return partner_.size();
	}

	void
	resize( Size const size_ )
	{
		partner_.resize( size_, 0 );
	}

	Size
	operator[]( Size const pos ) const
	{
		return partner_[ pos ];
	}

	Size &
	operator[]( Size const pos )
	{
		return partner_[ pos ];
	}

private:
	utility::vector1< Size > partner_;
};

/// helper fcn
BasePartner const &
retrieve_base_partner_from_pose( pose::Pose const & pose );


} // namespace dna
} // scoring
} // core

#endif
