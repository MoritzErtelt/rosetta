// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   basic/datacache/CacheableData.hh
/// @brief
/// @author Phil Bradley


#ifndef INCLUDED_basic_datacache_CacheableData_hh
#define INCLUDED_basic_datacache_CacheableData_hh

// unit headers
#include <basic/datacache/CacheableData.fwd.hh>

// utility headers
#include <utility/pointer/ReferenceCount.hh>


namespace basic {
namespace datacache {


/// @brief base class for data storable within a DataCache
class CacheableData : public utility::pointer::ReferenceCount {

public:
	virtual
	~CacheableData() {}

	virtual CacheableDataOP clone() const = 0;
};


} // namespace datacache
} // namespace basic

#endif /* INCLUDED_basic_datacache_CacheableData_HH */
