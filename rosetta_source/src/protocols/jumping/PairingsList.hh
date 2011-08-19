// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/protocols/jumping/PairingTemplate
/// @brief header file for ClassicAbinitio protocol
/// @detailed
///  from converting jumping_pairings.cc of rosetta++ into mini
///
/// @author Oliver Lange
/// @author Christopher Miles (cmiles@uw.edu)

#ifndef INCLUDED_protocols_jumping_PairingsList_hh
#define INCLUDED_protocols_jumping_PairingsList_hh

// Unit Headers
#include <protocols/jumping/PairingsList.fwd.hh>

// Project Headers
#include <core/types.hh>

// Utility headers
// we need it because of declarations in fwd.hh
#include <utility/vector1.hh>
#include <utility/exit.hh>

// ObjexxFCL Headers
#include <ObjexxFCL/FArray1A.fwd.hh>

//// C++ headers
#include <cstdlib> //required by GCC 4.3.2
#include <string>

namespace protocols {
namespace jumping {

using core::Size;

class Pairing {
 public:
	Pairing() :
		pos1_( 0 ),
		pos2_( 0 ),
		orientation_( 0 ),
		pleating_( 0 )
	{}

	Pairing( core::Size pos1_in, core::Size pos2_in ) :
		pos1_( pos1_in),
		pos2_( pos2_in),
		orientation_( 0 ),
		pleating_( 0 )
	{}

	//c'stor to translate from old-style version of pairing
	Pairing( ObjexxFCL::FArray1A_int );

	Pairing( core::Size pos1_in, core::Size pos2_in, core::Size ori_in, core::Size pleat_in ) :
		pos1_( pos1_in),
		pos2_( pos2_in),
		orientation_( ori_in),
		pleating_( pleat_in )
	{}

	Size Pos1() const {
    return pos1_;
	}

	void Pos1(Size pos1) {
		pos1_ = pos1;
	}

  Size Pos2() const {
		return pos2_;
	}

	void Pos2(Size pos2) {
		pos2_ = pos2;
	}

	Size Orientation() const {
		return orientation_;
	}

	void Orientation(Size orientation) {
		orientation_ = orientation;
	}

	Size Pleating() const {
		return pleating_;
	}

	void Pleating(Size pleating) {
		pleating_ = pleating;
	}

	///@brief constant values that define orientation
	static core::Size const ANTI = 1;
	static core::Size const PARALLEL = 2;
	static core::Size const OUTWARDS = 1;
	static core::Size const INWARDS = 2;

	///@brief reverses the Pairing
	Pairing reverse();

	///@brief returns a new reversed pairing
	Pairing generate_reversed() const;

	///
	bool is_parallel() const {
		runtime_assert( orientation_ );
		return orientation_ == PARALLEL;
	}

	///
	bool is_anti() const {
		runtime_assert( orientation_ );
		return orientation_ == ANTI;
	}

	bool is_inwards() const {
		runtime_assert( pleating_ );
		return pleating_ == INWARDS;
	}

	bool is_outwards() const {
		runtime_assert( pleating_ );
		return pleating_ == OUTWARDS;
	}

	bool operator ==( Pairing const& p ) const {
		return ( (p.Pos1() == Pos1())
					&& ( p.Pos2() == Pos2() )
				  && ( p.Orientation() == Orientation() )
				  && ( p.Pleating() == Pleating() )
		);
	}

	Size get_register()  {
		return is_anti() ? Pos1() + Pos2() : std::abs( (int) Pos1() - (int) Pos2() );
	}

	bool operator < ( Pairing const& p ) const {
		return p.Pos1() != Pos1() ? Pos1() < p.Pos1() :
			( p.Pos2() != Pos2() ? Pos2() < p.Pos2() :
				( p.Orientation() != Orientation() ? Orientation() < p.Orientation() : Pleating() < p.Pleating() ) );
	}

 private:
  Size pos1_;
  Size pos2_;
  Size orientation_;
  Size pleating_;
};

///@brief list of pairings
extern std::ostream& operator<< ( std::ostream& out, Pairing const& );
extern std::ostream& operator<< ( std::ostream& out, PairingsList const& p);

///@brief add pairings in pairing_file to list "pairings"
extern void read_pairing_list( std::string pairing_file, PairingsList& pairings);
extern void read_pairing_list( std::istream &is, PairingsList& pairings);

extern bool has_orientation_and_pleating( PairingsList const& );
} //protocols
} //jumping

#endif

