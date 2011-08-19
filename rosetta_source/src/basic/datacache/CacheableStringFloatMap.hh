// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   basic/datacache/CacheableStringFloatMap.hh
/// @brief
/// @author Phil Bradley


#ifndef INCLUDED_basic_datacache_CacheableStringFloatMap_hh
#define INCLUDED_basic_datacache_CacheableStringFloatMap_hh

// unit headers
#include <basic/datacache/CacheableStringFloatMap.fwd.hh>

// package headers
#include <basic/datacache/CacheableData.hh>

// C++ headers
#include <map>
#include <string>


namespace basic {
namespace datacache {


///@brief Wrapper for std::map< std::string, float >
class CacheableStringFloatMap : public CacheableData
{
public:
	CacheableStringFloatMap() : CacheableData() {}
	virtual ~CacheableStringFloatMap(){};
	virtual CacheableDataOP clone() const { return new CacheableStringFloatMap(*this); }

	virtual std::map< std::string, float > & map(){ return map_; }
	virtual const std::map< std::string, float > & map() const { return map_; }
private:
	std::map< std::string, float > map_;
};


} // namespace datacache
} // namespace basic

#endif /* INCLUDED_basic_datacache_CacheableStringFloatMap_HH */
