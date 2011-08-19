// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   src/core/unti/OStream.hh
/// @brief  base ostream like class for mini
/// @author Sergey Lyskov

#ifndef INCLUDED_utility_stream_util_hh
#define INCLUDED_utility_stream_util_hh

// AUTO-REMOVED #include <utility/vector1.hh>

#include <map>
#include <ostream>

//Auto Headers
#include <utility/vector1.fwd.hh>
#include <vector>


namespace utility {

} // namespace basic



/// -------------------------------------------------------------------
/// Predefined functions for Tracer IO
/// We move them out of name spaces so we can use them right away - without specifying it.


/// @brief Output function for std::vector object.
template <class T>
std::ostream & operator <<( std::ostream & os, std::vector<T> const & v)
{
	os << "[";
	for(size_t i=0; i<v.size(); i++) {
		os << v[i] << ", ";
	}
	os << "]";
	return os;
}


/// @brief Output function for utility::vector1 object.
template <class T>
std::ostream & operator <<(std::ostream & os, utility::vector1<T> const & v) {
	os << "[";
	for(size_t i=1; i<=v.size(); i++) {
		os << v[i] << ", ";
	}
	os << "]";
	return os;
}


/// @brief Output function for std::map object.
template <typename T1, typename T2>
std::ostream & operator <<(std::ostream & os, std::map<T1, T2> const & m) {
	typedef typename std::map<T1, T2>::const_iterator ConstIterator;
	ConstIterator p;

	os << "{";

	for(p=m.begin(); p!=m.end(); p++ ) {
		os << p->first << ":" << p->second << ", ";
	}

	os << "}";
	return os;
}


#endif // INCLUDED_utility_stream_util_hh
