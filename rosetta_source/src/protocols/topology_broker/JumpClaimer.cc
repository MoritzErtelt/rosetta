// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file TopologyBroker
/// @brief  top-class (Organizer) of the TopologyBroker mechanism
/// @detailed responsibilities:
/// @author Oliver Lange

// Unit Headers
#include <protocols/topology_broker/JumpClaimer.hh>

// Package Headers
#include <protocols/topology_broker/DofClaim.hh>

// Project Headers
#include <core/pose/Pose.hh>
#include <core/kinematics/MoveMap.hh>
#ifdef WIN32
#include <core/fragment/FragID.hh>
#endif

#include <core/fragment/FragSet.hh>
#include <core/fragment/OrderedFragSet.hh>
#include <core/fragment/FrameList.hh>
// AUTO-REMOVED #include <core/fragment/FragmentIO.hh>
#include <protocols/basic_moves/FragmentMover.hh>
#include <protocols/jumping/JumpSetup.hh>



// ObjexxFCL Headers

// Utility headers
//#include <utility/io/izstream.hh>
//#include <utility/io/ozstream.hh>
#include <utility/excn/Exceptions.hh>
#include <basic/Tracer.hh>
//#include <basic/options/option.hh>

//// C++ headers
// AUTO-REMOVED #include <fstream>

//Auto Headers
#include <core/fragment/Frame.hh>


// option key includes

static basic::Tracer tr("protocols.topo_broker",basic::t_info);
//static numeric::random::RandomGenerator RG(18828234);

namespace protocols {
namespace topology_broker {

using namespace core;

JumpClaimer::JumpClaimer() :
	jump_def_( NULL ),
	init_mover_( NULL ),
	bKeepJumpsFromInputPose_( true )
{
	set_bInitDofs( true ); //we want to initialize jumps
}

JumpClaimer::JumpClaimer( jumping::BaseJumpSetupOP jump_def, std::string const& tag, weights::AbinitioMoverWeightOP weight ) :
	FragmentClaimer( NULL, tag, weight ),
	jump_def_ ( jump_def ),
	init_mover_( NULL ),
	bKeepJumpsFromInputPose_( true )
{
	set_bInitDofs( true ); //we want to initialize jumps
}

JumpClaimer::JumpClaimer( JumpClaimer const & src ) :
	TopologyClaimer( src ),
	Parent( src )
{
	jump_def_ = src.jump_def_ ;
	current_jumps_ = src.current_jumps_ ;
	init_mover_ = src.init_mover_ ;
	bKeepJumpsFromInputPose_ = src.bKeepJumpsFromInputPose_ ;

}

JumpClaimer::~JumpClaimer() {}

TopologyClaimerOP JumpClaimer::clone() const
{
	return new JumpClaimer( *this );
}


void JumpClaimer::new_decoy() {
	discard_jumps_ = true;
	input_pose_.clear();
}


void JumpClaimer::new_decoy( core::pose::Pose const& pose ) {
	new_decoy();
	input_pose_ = pose;
}

void JumpClaimer::initialize_dofs( core::pose::Pose& pose, DofClaims const& init_dofs, DofClaims& failed_to_init ) {
	//need to copy coords and jumps --- if chunks were idealized no problem .... but non-idealized stuff ?
	if ( init_mover_ ) {
		basic_moves::FragmentMoverOP frag_mover = get_frag_mover_ptr();
		set_mover( init_mover_ );
		FragmentClaimer::initialize_dofs( pose, init_dofs, failed_to_init );
		set_mover( frag_mover );
		init_mover_ = NULL;
	} else {
		FragmentClaimer::initialize_dofs( pose, init_dofs, failed_to_init );
	}
}

void JumpClaimer::init_jumps() {
	discard_jumps_ = false;
	if ( bKeepJumpsFromInputPose_ && input_pose_.total_residue() > 0 ) {
		tr.Info << type()
						<< ": get jumps from input pose. use flag NO_USE_INPUT_POSE if you want to create new jumps "
						<< std::endl;
		current_jumps_ = jumping::JumpSample( input_pose_.fold_tree() );
		if ( current_jumps_.size() == 0 ) return;
		std::set< Size > active_region;
		get_sequence_region( active_region );
		ObjexxFCL::FArray2D_int filtered_jumps( 2, current_jumps_.size() );
		ObjexxFCL::FArray1D_int filtered_cuts( current_jumps_.size() );
		Size ct( 0 );
		for ( Size i = 1; i<=current_jumps_.size(); ++i ) {
			Size jump1( current_jumps_.jumps()( 1, i ) );
			Size jump2( current_jumps_.jumps()( 2, i ) );
			if ( active_region.find( jump1 ) != active_region.end()
				&& active_region.find( jump2 ) != active_region.end() ) {
				++ct;
				filtered_jumps( 1, ct ) =  jump1 < jump2 ? jump1 : jump2 ;
				filtered_jumps( 2, ct ) =  jump1 < jump2 ? jump2 : jump1 ;
				filtered_cuts( ct ) = current_jumps_.cuts()( i );
			}
		}
		current_jumps_ = jumping::JumpSample( *active_region.rbegin() /*total_residue*/, ct, filtered_jumps, filtered_cuts );

		if ( jump_def_ ) current_jumps_ = jump_def_->clean_jumps( current_jumps_ );
		tr.Debug << "current_jumps " << current_jumps_ << std::endl;
	} else {
		if ( !jump_def_ ) return;
		runtime_assert( jump_def_ );
		tr.Info << type() << ": create new random jumps" << std::endl;
		Size attempts( 10 );
		do {
			current_jumps_ = jump_def_->create_jump_sample();
		} while ( !current_jumps_.is_valid() && attempts-- );
	}

	if ( !current_jumps_.is_valid() ) {
		throw utility::excn::EXCN_BadInput("not able to build valid fold-tree in JumpClaimer");
	}
	tr.Debug << "current_jumps " << current_jumps_ << std::endl;

	if ( input_pose_.total_residue() > 0 ) {
		core::fragment::FrameList jump_frames;
		kinematics::MoveMap mm;
		mm.set_bb( true );
		mm.set_jump( true );

		current_jumps_.generate_jump_frames( jump_frames,mm );

		for ( core::fragment::FrameList::iterator jump_frame = jump_frames.begin();
					jump_frame != jump_frames.end(); ++jump_frame ) {
			(*jump_frame)->steal( input_pose_ );
		}

		core::fragment::FragSetOP jump_frags = new core::fragment::OrderedFragSet;
		jump_frags->add( jump_frames );

		init_mover_ = new basic_moves::ClassicFragmentMover( jump_frags, movemap_ );
		init_mover_->type( mover_tag() );
		init_mover_->set_check_ss( false ); // this doesn't make sense with jump fragments
		init_mover_->enable_end_bias_check( false ); //no sense for discontinuous fragments
	}
	input_pose_.clear();

}

void JumpClaimer::generate_claims( DofClaims& new_claims ) {
	if ( discard_jumps_ ) init_jumps();
	// get flexible jumps ( beta-sheet stuff etc. )
	/// in future get rid of JumpSample class all-together.

	movemap_->set_jump( true ); //we switch them off on a as-need basis
	movemap_->set_bb( true );

	fragment::FragSetOP jump_frags;
	if ( jump_def_ ) {
		jump_frags = jump_def_->generate_jump_frags( current_jumps_, *movemap_ );

		basic_moves::ClassicFragmentMoverOP jump_mover = new basic_moves::ClassicFragmentMover( jump_frags, movemap_ );
		jump_mover->type( mover_tag() );
		jump_mover->set_check_ss( false ); // this doesn't make sense with jump fragments
		jump_mover->enable_end_bias_check( false ); //no sense for discontinuous fragments
		set_mover( jump_mover );
	} else {
		jump_frags = new core::fragment::OrderedFragSet;
		core::fragment::FrameList jump_frames;
		current_jumps_.generate_jump_frames( jump_frames, *movemap_ );
		jump_frags->add( jump_frames );
	}

	Size nr_jumps = current_jumps_.size();
	//	runtime_assert( jump_frags->nr_frames() == nr_jumps );
	for ( Size i = 1; i<=nr_jumps; ++i ) {
		Size const up( current_jumps_.jumps()( 1, i ) );
		Size const down( current_jumps_.jumps()( 2, i ) );
		std::string up_atom( current_jumps_.jump_atoms()(1, i ) );
		std::string down_atom( current_jumps_.jump_atoms()(2, i ) );
		//now this assumes that we have the BB - Jump -  BB frags...
		//	how about translating Fragments directly into Claims ???
		fragment::FrameList frames;
		jump_frags->frames( up, frames );
		jump_frags->frames( down, frames );
		kinematics::MoveMap jump_mm;
		jump_mm.set_jump( up, down, true );
		bool found_frame( false );
		for ( fragment::FrameList::iterator it = frames.begin(); it!=frames.end(); ++it ) {
			if ( 2 == (*it)->nr_res_affected( jump_mm ) ) {
				//that is our jump-fragment
				found_frame = true;
				new_claims.push_back( new JumpClaim( this, up, down, up_atom, down_atom, DofClaim::INIT ) );
				kinematics::MoveMap bb_mm; bb_mm.set_bb( false );
				bb_mm.set_bb( up, true );
				if ( 2 == (*it)->nr_res_affected( bb_mm ) ) 	new_claims.push_back( new BBClaim( this, up ) ); //up jump always counted
				bb_mm.set_bb( down, true );		bb_mm.set_bb( up, false);
				if ( 2 == (*it)->nr_res_affected( bb_mm ) ) 	new_claims.push_back( new BBClaim( this, down ) ); //up jump always counted
				break;
			}
		}
		runtime_assert( found_frame ); // there should be fragments for each jump!
	} // for 1..nr_jumps
}

bool JumpClaimer::read_tag( std::string tag, std::istream& is ) {
	if ( tag == "NO_USE_INPUT_POSE" ) {
		bKeepJumpsFromInputPose_ = false;
	} else if ( tag == "USE_INPUT_POSE" ) {
		bKeepJumpsFromInputPose_ = true;
	} else return Parent::read_tag( tag, is );
	return true;
}

} //topology_broker
} //protocols
