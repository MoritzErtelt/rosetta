// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file       protocols/membrane/SpinAroundPartnerMover.hh
/// @brief      Spins the downstream partner around the upstream partner
/// @details	Spins the downstream partner around the upstream partner in the
///				membrane to probe all kinds of interfaces. Both embedding normals
///				are approximately conserved, i.e. the partners aren't flipped
///				in the membrane.
/// @author     JKLeman (julia.koehler1982@gmail.com)

#ifndef INCLUDED_protocols_membrane_SpinAroundPartnerMover_hh
#define INCLUDED_protocols_membrane_SpinAroundPartnerMover_hh

// Unit Headers
#include <protocols/membrane/SpinAroundPartnerMover.fwd.hh>
#include <protocols/membrane/SpinAroundPartnerMoverCreator.hh>
#include <protocols/moves/Mover.hh>

// Project Headers

// Package Headers
#include <core/pose/Pose.fwd.hh>
#include <core/types.hh> 
#include <protocols/rosetta_scripts/util.hh>
#include <protocols/filters/Filter.fwd.hh>

// Utility Headers
#include <utility/vector1.hh>
#include <numeric/xyzVector.hh>
#include <utility/tag/Tag.fwd.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <basic/Tracer.fwd.hh>

namespace protocols {
namespace membrane {

using namespace core;
using namespace core::pose;
using namespace protocols::moves;
	  
class SpinAroundPartnerMover : public protocols::moves::Mover {

public:

	/////////////////////
	/// Constructors  ///
	/////////////////////

	/// @brief Default Constructor
	/// @details Defaults: jump = 1, sampling range = 100
	///			 Sampling range of 100 means that both x and y are sampled from
	///			 -100 to +100 before calling DockingSlideIntoContact
	SpinAroundPartnerMover();
	
	/// @brief Custom Constructor
	/// @details User can specify jump number
	SpinAroundPartnerMover( Size jump_num );

	/// @brief Custom constructor
	/// @details User can specify jump number and sampling range
	SpinAroundPartnerMover( Size jump_num, Size range );

	/// @brief Copy Constructor
	SpinAroundPartnerMover( SpinAroundPartnerMover const & src );

	/// @brief Assignment Operator
	SpinAroundPartnerMover & operator = ( SpinAroundPartnerMover const & src );
	
	/// @brief Destructor
	virtual ~SpinAroundPartnerMover();
	
	///////////////////////////////
	/// Rosetta Scripts Methods ///
	///////////////////////////////
	
	/// @brief Create a Clone of this mover
	virtual protocols::moves::MoverOP clone() const;
	
	/// @brief Create a Fresh Instance of this Mover
	virtual protocols::moves::MoverOP fresh_instance() const;
	
	/// @brief Pase Rosetta Scripts Options for this Mover
	void parse_my_tag(
	  utility::tag::TagCOP tag,
	  basic::datacache::DataMap &,
	  protocols::filters::Filters_map const &,
	  protocols::moves::Movers_map const &,
	  core::pose::Pose const &
	);
	
	/////////////////////
	/// Mover Methods ///
	/////////////////////
	
	/// @brief Get the name of this Mover (SpinAroundPartnerMover)
	virtual std::string get_name() const;
		
	/// @brief Flip the downstream partner in the membrane
	virtual void apply( Pose & pose );
	
	/// @brief Set random range
	void random_range( bool yesno );
	
	/// @brief Set x position
	void set_x( Real x );
	
	/// @brief Set y position
	void set_y( Real y );
	
private: // methods
	
	/////////////////////
	/// Setup Methods ///
	/////////////////////

	/// @brief Register Options from Command Line
	void register_options();
	
	/// @brief Set default values
	void set_defaults();
	
private: // data

	/// @brief Jump number
	Size jump_;
	
	// @brief Random range
	bool rand_range_;
	
	/// @brief Sampling range
	Size range_;
	
	/// @brief Set x and y position
	Real x_;
	Real y_;

};

} // membrane
} // protocols

#endif // INCLUDED_protocols_membrane_SpinAroundPartnerMover_hh
