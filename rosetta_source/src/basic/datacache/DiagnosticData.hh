// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   basic/datacache/DiagnosticData.hh
/// @brief
/// @author Phil Bradley


#ifndef INCLUDED_basic_datacache_DiagnosticData_hh
#define INCLUDED_basic_datacache_DiagnosticData_hh

// unit headers
#include <basic/datacache/DiagnosticData.fwd.hh>

// type headers

// package headers
#include <basic/datacache/CacheableData.hh>

// C++ headers
#include <map>
#include <string>


namespace basic {
namespace datacache {


///@brief Wrapper for std::map<string, Real>
class DiagnosticData : public CacheableData
{
public:
	DiagnosticData( std::map < std::string, double >  data_in ) : CacheableData(), data_(data_in) {}
	virtual ~DiagnosticData(){};
	virtual CacheableDataOP clone() const { return new DiagnosticData(*this); }
	virtual std::map < std::string, double > const & data() const { return data_; }
private:
	std::map < std::string, double > data_;
};


} // namespace datacache
} // namespace basic

#endif /* INCLUDED_basic_datacache_DiagnosticData_HH */
