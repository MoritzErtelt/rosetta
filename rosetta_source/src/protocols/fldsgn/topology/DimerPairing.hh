// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is made available under the Rosetta Commons license.
// See http://www.rosettacommons.org/license
// (C) 199x-2007 University of Washington
// (C) 199x-2007 University of California Santa Cruz
// (C) 199x-2007 University of California San Francisco
// (C) 199x-2007 Johns Hopkins University
// (C) 199x-2007 University of North Carolina, Chapel Hill
// (C) 199x-2007 Vanderbilt University

/// @file
/// @brief
/// @author Nobuyasu Koga ( nobuyasu@uw.edu )

#ifndef INCLUDED_protocols_fldsgn_topology_DimerPairing_hh
#define INCLUDED_protocols_fldsgn_topology_DimerPairing_hh

#include <core/types.hh>
#include <utility/pointer/ReferenceCount.hh>
#include <protocols/fldsgn/topology/DimerPairing.fwd.hh>
#include <protocols/fldsgn/topology/SS_Info2.fwd.hh>
#include <utility/vector1.hh>

namespace protocols {
namespace fldsgn {
namespace topology {


class DimerPairing  : public utility::pointer::ReferenceCount {
public:


	typedef core::Size Size;
	typedef core::Real Real;


public:

	DimerPairing (
		Size const res1,
		Size const res2,
		Real const dist,
		Real const phi,
		Real const theta,
		Real const sigma,
		Real const dp,
		Size const sign1,
		Size const sign2,
		Real const score );

	Size
	res1() const
	{
		return res1_;
	}

	Size
	res2() const
	{
		return res2_;
	}

	Real
	dist() const
	{
		return dist_;
	}

	Real
	phi() const
	{
		return phi_;
	}

	Real
	theta() const
	{
		return theta_;
	}

	Real
	dp() const
	{
		return dp_;
	}

	Real
	sigma() const
	{
		return sigma_;
	}

	Size
	sign1() const
	{
		return sign1_;
	}

	Size
	sign2() const
	{
		return sign2_;
	}

	Real
	score() const
	{
		return score_;
	}

	char
	orient() const
	{
		return orient_;
	}

	bool
	valid() const
	{
		return valid_;
	}

	void
	valid( bool const v )
	{
		valid_ = v;
	}



public:


	bool is_parallel( Real const phi, Real const theta );


public:


	friend
	std::ostream & operator<<( std::ostream & out, const DimerPairing &dp );


private: // data


	Size res1_;
	Size res2_;
	Real dist_;
	Real phi_;
	Real theta_;
	Real sigma_;
	Real dp_;
	Size sign1_;
  Size sign2_;
	Real score_;
	char orient_;
	bool valid_;

};

class DimerPairings : public utility::vector1< DimerPairingOP > {
public:

	typedef core::Size Size;
	typedef protocols::fldsgn::topology::SS_Info2 SS_Info2;
	typedef protocols::fldsgn::topology::DimerPairing DimerPairing;

public:

	void finalize( SS_Info2 const & ss_info );

	friend
	std::ostream & operator<<( std::ostream& out, const DimerPairings &dps );

};


} // ns topology
} // ns fldsgn
} // ns protocols

#endif
