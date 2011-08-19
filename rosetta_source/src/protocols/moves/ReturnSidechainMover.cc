// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/moves/ReturnSidechainMover.cc
/// @brief ReturnSidechainMover methods implemented
/// @author Steven Lewis


// Unit Headers
#include <protocols/moves/ReturnSidechainMover.hh>

// Package Headers

// Project Headers
// AUTO-REMOVED #include <core/conformation/Conformation.hh>
#include <core/pose/Pose.hh>

#include <core/scoring/Energies.hh>

// AUTO-REMOVED #include <core/chemical/VariantType.hh>

#include <core/chemical/ResidueType.hh>

// Utility Headers
#include <basic/Tracer.hh>
#include <core/types.hh>
#include <utility/exit.hh>

// C++ Headers
#include <string> //making sure that residue_type.name() comparison works

//Auto Headers
#include <core/conformation/Residue.hh>
#include <core/kinematics/Jump.hh>
#include <core/pose/util.hh>


using basic::T;
using basic::Error;
using basic::Warning;


static basic::Tracer TR( "protocols.moves.ReturnSidechainMover" );


namespace protocols {
namespace moves {

///@details this code was copied from protocols/loops/loops_main.cc:187-204, revision 21282
void ReturnSidechainMover::apply( core::pose::Pose & pose ){

	core::pose::Pose saved_input_pose(remembered_pose_);
	core::Size nres( end_res_ - start_res_ + 1 );

	if( nres != saved_input_pose.total_residue() )
		utility_exit_with_message("ReturnSidechainMover used with poses of different length; aborting");

	for (Size i=start_res_, j=1; i<= end_res_; ++i, ++j ) {
		bool copy_this_residue( false );

		if( copy_all_chi_ )
			copy_this_residue = true;
		else {
			if( allow_chi_copy_[i] )
				copy_this_residue = true;
		}

		if (pose.residue_type(i).aa() == core::chemical::aa_vrt) continue;

		if( copy_this_residue ) {
			core::chemical::ResidueType const & rsd_type ( pose.residue(i).type() );
			core::chemical::ResidueType const & saved_rsd_type( saved_input_pose.residue(j).type() );

			//ensure that there is no sequence change
			if( rsd_type.name3() != saved_rsd_type.name3() )
				utility_exit_with_message("ReturnSidechainMover used with poses of different sequence; aborting");

			//we need to check variant types in case there are cutpoints for loop modeling or whatever
			if ( ! rsd_type.variants_match( saved_rsd_type) ) {
				utility::vector1<core::chemical::VariantType> const & variant_types ( rsd_type.variant_types() );
				utility::vector1<core::chemical::VariantType> missing_variant_types;

				for ( utility::vector1<core::chemical::VariantType>::const_iterator it = variant_types.begin(),
								it_end=variant_types.end(); it != it_end; ++it ) {
					if ( !saved_rsd_type.has_variant_type( *it ) ) missing_variant_types.push_back( *it );
				}

				for ( utility::vector1<core::chemical::VariantType>::const_iterator it = missing_variant_types.begin(),
								it_end=missing_variant_types.end(); it != it_end; ++it ) {
					core::pose::add_variant_type_to_pose_residue( saved_input_pose, *it, i);
				}
			}//checking variants

			pose.replace_residue(i, saved_input_pose.residue(j), true );
		}
	} // residue i

	pose.energies().clear();
	return;
}//apply

std::string
ReturnSidechainMover::get_name() const {
	return "ReturnSidechainMover";
}

///@brief default constructor
ReturnSidechainMover::ReturnSidechainMover() : Mover()
{
	Mover::type( "ReturnSidechainMover" );
	copy_all_chi_ = true;
}

///@brief constructor with pose
ReturnSidechainMover::ReturnSidechainMover(
	core::pose::Pose const & pose_in,
	core::Size start_res,
	core::Size end_res ) :
	Mover(), remembered_pose_(pose_in)
{
	Mover::type( "ReturnSidechainMover" );
	copy_all_chi_ = true;
	if ( start_res == 0 ) start_res_ = 1;
	else start_res_ = start_res;
	if ( end_res == 0 ) end_res_ = pose_in.total_residue();
	else end_res_ = end_res;
}

///@brief constructor with pose
ReturnSidechainMover::ReturnSidechainMover(
	core::pose::Pose const & pose_in,
	utility::vector1<bool> allow_chi_in,
	core::Size start_res,
	core::Size end_res ) :
	Mover(),
	allow_chi_copy_( allow_chi_in ),
	remembered_pose_(pose_in)
{
	Mover::type( "ReturnSidechainMover" );
	copy_all_chi_ = false;
	if ( start_res == 0 ) start_res_ = 1;
	else start_res_ = start_res;
	if ( end_res == 0 ) end_res_ = pose_in.total_residue();
	else end_res_ = end_res;
}

ReturnSidechainMover::~ReturnSidechainMover() {}

}//moves
}//protocols
