// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/antibody/H3RefineCCD.cc
/// @brief Build a homology model of an antibody
/// @details
///
///
/// @author Jianqing Xu (xubest@gmail.com)

#include <basic/Tracer.hh>

#include <core/chemical/VariantType.hh>

#include <core/kinematics/MoveMap.hh>
#include <core/pack/task/operation/NoRepackDisulfides.fwd.hh>
#include <core/pack/task/TaskFactory.hh>
#include <core/pose/Pose.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <protocols/antibody/H3RefineCCD.hh>
#include <protocols/antibody/util.hh>
#include <protocols/loops/Loop.hh>
#include <protocols/loops/loops_main.hh>
#include <protocols/loops/loop_closure/ccd/CCDLoopClosureMover.hh>
#include <protocols/moves/ChangeFoldTreeMover.hh>
#include <protocols/moves/MonteCarlo.hh>
#include <protocols/moves/MoverContainer.hh>
#include <protocols/moves/RepeatMover.hh>
#include <protocols/simple_moves/BackboneMover.hh>
#include <protocols/minimization_packing/MinMover.hh>
#include <protocols/minimization_packing/PackRotamersMover.hh>
#include <protocols/minimization_packing/RotamerTrialsMover.hh>
#include <protocols/simple_task_operations/RestrictToInterface.hh>

#include <numeric/numeric.functions.hh>

#include <protocols/antibody/AntibodyInfo.hh> // AUTO IWYU For AntibodyInfo

static basic::Tracer TR( "protocols.antibody.H3RefineCCD" );

using namespace core;

namespace protocols {
namespace antibody {


// default constructor
H3RefineCCD::H3RefineCCD( ) : Mover() {}

H3RefineCCD::H3RefineCCD( AntibodyInfoOP antibody_info, CDRNameEnum loop_name ) : Mover() {
	user_defined_ = false;
	ab_info_    = antibody_info;
	loop_name_  = loop_name;

	init();
}


H3RefineCCD::H3RefineCCD(AntibodyInfoOP antibody_info,
	CDRNameEnum loop_name,
	scoring::ScoreFunctionCOP highres_scorefxn
) : Mover() {
	user_defined_ = true;
	ab_info_    = antibody_info;
	loop_name_  = loop_name;
	highres_scorefxn_ = highres_scorefxn->clone();

	init();
}

H3RefineCCD::H3RefineCCD( AntibodyInfoOP antibody_info ) : Mover() {
	ab_info_ = antibody_info;

	init();
}


void H3RefineCCD::init( ) {
	set_default();

	the_loop_   = ab_info_->get_CDR_loop(loop_name_);

	loop_begin_ = the_loop_.start();
	cutpoint_   = the_loop_.cut();
	loop_end_   = the_loop_.stop();
	loop_size_  = ( loop_end_ - loop_begin_ ) + 1;

	gamma_ = std::pow( (last_temp_/init_temp_), (1.0/loop_size_)); //TODO: check this gama value carefully

	n_small_moves_ =  numeric::max(core::Size(5), core::Size(loop_size_/2)) ;
	inner_cycles_ = loop_size_;
	outer_cycles_ = 2; //JQX: assume the SnugFit step has done some minimization
	if (  refine_input_loop_ ) {
		outer_cycles_ = 5;
	}
	if ( benchmark_ ) {
		min_tolerance_ = 1.0;
		n_small_moves_ = 1;
		inner_cycles_ = 1;
		outer_cycles_ = 1;
	}

}


void H3RefineCCD::set_default() {
	include_neighbors_ = true;
	H3_filter_         = false; // use the kink constraint insted, this is deprecated
	flank_relax_       = true;

	benchmark_         = false;
	refine_input_loop_ = false;
	is_camelid_        = false;

	neighbor_dist_  = 10.0;
	flank_size_     = 2;
	min_tolerance_  = 0.001;
	high_move_temp_ = 2.00;

	init_temp_      = 2.0;
	last_temp_      = 0.5;

	high_cst_       = 100.0;
	max_cycle_close_trial_ = 20;
	minimization_type_ = "lbfgs_armijo_nonmonotone" ;


	if ( !user_defined_ ) {
		highres_scorefxn_ = scoring::get_score_function();
		highres_scorefxn_->set_weight( scoring::chainbreak, 1.0 );
		highres_scorefxn_->set_weight( scoring::overlap_chainbreak, 10./3. );
		highres_scorefxn_->set_weight( scoring::atom_pair_constraint, high_cst_ );
	}

}


// default destructor
H3RefineCCD::~H3RefineCCD() = default;

//clone
protocols::moves::MoverOP H3RefineCCD::clone() const {
	return( utility::pointer::make_shared< H3RefineCCD >() );
}


std::string H3RefineCCD::get_name() const {
	return "H3RefineCCD";
}


void H3RefineCCD::pass_start_pose(core::pose::Pose & start_pose) {
	start_pose_ = start_pose;
}


void H3RefineCCD::finalize_setup( core::pose::Pose & pose ) {

	// score the pose first
	(*highres_scorefxn_) (pose);


	// the list of residues that are allowed to pack
	for ( core::Size ii=1; ii <=pose.size(); ii++ ) {
		allow_repack_.push_back(false);
	}
	select_loop_residues( pose, the_loop_, include_neighbors_, allow_repack_, neighbor_dist_);


	// the list of residues that are allowed to change backbone
	utility::vector1< bool> allow_bb_move( pose.size(), false );
	for ( core::Size ii = loop_begin_; ii <= loop_end_; ii++ ) {
		allow_bb_move[ ii ] = true;
	}

	// the movemap of the h3 loop, if flank_relax_=false, flank_cdrh3_map_=cdrh3_map_
	cdrh3_map_ = utility::pointer::make_shared< kinematics::MoveMap >();
	cdrh3_map_->set_jump( 1, false );
	cdrh3_map_->set_bb( allow_bb_move );
	cdrh3_map_->set_chi( allow_repack_ );

	if ( flank_relax_ ) {
		utility::vector1< bool> flank_allow_bb_move( allow_bb_move );
		for ( core::Size i = 1; i <= pose.size(); i++ ) {
			if (  (i >= (loop_begin_ - flank_size_)) && (i <= (loop_end_ + flank_size_))   ) {
				flank_allow_bb_move[i] = true;
			}
		}

		flank_cdrh3_map_ = utility::pointer::make_shared< kinematics::MoveMap >();
		flank_cdrh3_map_->set_jump( 1, false );
		flank_cdrh3_map_->set_bb( flank_allow_bb_move );
		flank_cdrh3_map_->set_chi( allow_repack_ );
	} else {
		flank_cdrh3_map_ = cdrh3_map_;
	}


	// below are the definitions of a bunch of movers
	using namespace protocols;
	using namespace protocols::simple_moves;
	using namespace protocols::minimization_packing;
	using namespace protocols::loops;
	using namespace protocols::moves;
	using namespace protocols::simple_task_operations;
	using namespace pack;
	using namespace pack::task;
	using namespace pack::task::operation;
	using loop_closure::ccd::CCDLoopClosureMover;
	using loop_closure::ccd::CCDLoopClosureMoverOP;


	// the Monte Carlo mover
	mc_ = utility::pointer::make_shared< protocols::moves::MonteCarlo >( pose, *highres_scorefxn_, init_temp_ );


	// setup some easy objects to change the fold trees
	// if not flank_relax =false, then   change_FT_to_flankloop_ = change_FT_to_simpleloop_
	simple_fold_tree( pose, loop_begin_ - 1, cutpoint_, loop_end_ + 1 );
	change_FT_to_simpleloop_ = utility::pointer::make_shared< ChangeFoldTreeMover >( pose.fold_tree() );

	if ( flank_relax_ ) {
		simple_fold_tree( pose, loop_begin_ - flank_size_ - 1, cutpoint_, loop_end_ + flank_size_ + 1 );
	}
	change_FT_to_flankloop_ = utility::pointer::make_shared< ChangeFoldTreeMover >( pose.fold_tree() );

	// cut points variants for chain-break scoring
	loops::remove_cutpoint_variants( pose, true ); //remove first
	loops::add_cutpoint_variants( pose );


	// pack the loop and its neighboring residues
	loop_repack_ = utility::pointer::make_shared< PackRotamersMover >(highres_scorefxn_);
	tf_ = setup_packer_task( start_pose_);
	( *highres_scorefxn_ )( pose );
	tf_->push_back( utility::pointer::make_shared< RestrictToInterface >( allow_repack_ ) );
	loop_repack_->task_factory(tf_);
	//loop_repack_->apply( pose_in );


	// minimize amplitude of moves if correct parameter is set
	BackboneMoverOP small_mover( new SmallMover( cdrh3_map_, high_move_temp_, n_small_moves_ ) );
	BackboneMoverOP shear_mover( new ShearMover( cdrh3_map_, high_move_temp_, n_small_moves_ ) );

	small_mover->angle_max( 'H', 2.0 );
	small_mover->angle_max( 'E', 5.0 );
	small_mover->angle_max( 'L', 6.0 );

	shear_mover->angle_max( 'H', 2.0 );
	shear_mover->angle_max( 'E', 5.0 );
	shear_mover->angle_max( 'L', 6.0 );


	// ccd moves
	CCDLoopClosureMoverOP ccd_moves( new CCDLoopClosureMover( the_loop_, cdrh3_map_ ) );
	RepeatMoverOP ccd_cycle( new RepeatMover(ccd_moves, n_small_moves_) );


	// minimization mover
	loop_min_mover_ = utility::pointer::make_shared< MinMover >( flank_cdrh3_map_, highres_scorefxn_, minimization_type_, min_tolerance_, true /*nb_list*/ );


	// put everything into a sequence mover
	wiggle_cdr_h3_ = utility::pointer::make_shared< SequenceMover >() ;
	wiggle_cdr_h3_->add_mover( change_FT_to_simpleloop_ );
	wiggle_cdr_h3_->add_mover( small_mover );
	wiggle_cdr_h3_->add_mover( shear_mover );
	wiggle_cdr_h3_->add_mover( ccd_cycle );
	wiggle_cdr_h3_->add_mover( change_FT_to_flankloop_ );
	wiggle_cdr_h3_->add_mover( loop_min_mover_ );
}


//APPLY
void H3RefineCCD::apply( pose::Pose & pose ) {
	using namespace protocols::simple_moves;
	using namespace protocols::minimization_packing;
	using namespace protocols::moves;
	using namespace protocols::simple_task_operations;
	using namespace pack::task;
	using namespace pack::task::operation;

	/// FIXME: This can cause issues if using the default refine_method in RefineOneCDRLoop and a loop in centroid mode!
	/// This can happen in SnugDock.
	if ( !pose.is_fullatom() ) {
		utility_exit_with_message("Fullatom poses only");
	}

	TR <<  " Relaxing Fullatom CDR H3 loop" << std::endl;


	finalize_setup(pose);


	bool closed_cutpoints( false );
	core::Size cycle( 1 );
	while ( !closed_cutpoints && cycle<max_cycle_close_trial_ ) {
		TR <<  "    Refining CDR H3 loop in HighRes.  close_trial_cycle="<<cycle << std::endl;


		change_FT_to_flankloop_->apply(pose);
		loop_min_mover_->apply(pose);

		// rotamer trials
		select_loop_residues( pose, the_loop_, include_neighbors_, allow_repack_, neighbor_dist_);
		tf_ = setup_packer_task( start_pose_ );
		( *highres_scorefxn_ )( pose );
		tf_->push_back( utility::pointer::make_shared< RestrictToInterface >( allow_repack_ ) );
		RotamerTrialsMoverOP pack_rottrial( new RotamerTrialsMover( highres_scorefxn_, tf_ ) );
		pack_rottrial->apply( pose );


		Real temperature = init_temp_;
		mc_->reset( pose ); // monte carlo reset

		bool relaxed_H3_found_ever( false );
		if ( H3_filter_ ) {
			relaxed_H3_found_ever = CDR_H3_cter_filter( pose,ab_info_);
		}

		// outer cycle
		for ( core::Size i = 1; i <= outer_cycles_; i++ ) {
			mc_->recover_low( pose );
			core::Size h3_attempts(0); // number of H3 checks after refinement

			// inner cycle
			for ( core::Size j = 1; j <= inner_cycles_; j++ ) {
				mc_->set_temperature( temperature*= gamma_ );

				wiggle_cdr_h3_->apply( pose );

				// rotamer trials
				select_loop_residues( pose, the_loop_, include_neighbors_, allow_repack_, neighbor_dist_);
				tf_ = setup_packer_task( start_pose_);
				( *highres_scorefxn_ )( pose );
				tf_->push_back( utility::pointer::make_shared< RestrictToInterface >( allow_repack_ ) );
				pack_rottrial->task_factory(tf_);
				pack_rottrial->apply( pose );

				//bool relaxed_H3_found_current(false);
				// H3 filter check
				if ( H3_filter_ && (h3_attempts <= inner_cycles_) ) {
					h3_attempts++;
					bool relaxed_H3_found_current = CDR_H3_cter_filter( pose,ab_info_);

					if ( !relaxed_H3_found_ever && !relaxed_H3_found_current ) {
						mc_->boltzmann( pose );
					} else if ( !relaxed_H3_found_ever && relaxed_H3_found_current ) {
						relaxed_H3_found_ever = true;
						mc_->reset( pose );
					} else if ( relaxed_H3_found_ever && !relaxed_H3_found_current ) {
						--j;
						continue;
					} else if ( relaxed_H3_found_ever && relaxed_H3_found_current ) {
						mc_->boltzmann( pose );
					}
				} else {
					if ( H3_filter_ ) {
						bool relaxed_H3_found_current(false);
						relaxed_H3_found_current = CDR_H3_cter_filter( pose,ab_info_);
						if ( !relaxed_H3_found_ever && !relaxed_H3_found_current ) {
							mc_->boltzmann( pose );
						} else if ( !relaxed_H3_found_ever && relaxed_H3_found_current ) {
							relaxed_H3_found_ever = true;
							mc_->reset( pose );
						} else if ( relaxed_H3_found_ever && !relaxed_H3_found_current ) {
							mc_->recover_low( pose );
						} else if ( relaxed_H3_found_ever && relaxed_H3_found_current ) {
							mc_->boltzmann( pose );
						}
					} else {
						mc_->boltzmann( pose );
					}
				}

				if ( numeric::mod(j,core::Size(20))==0 || j==inner_cycles_ ) {
					// repack trial
					loop_repack_ = utility::pointer::make_shared< PackRotamersMover >( highres_scorefxn_ );
					tf_ = setup_packer_task( start_pose_);
					( *highres_scorefxn_ )( pose );
					tf_->push_back( utility::pointer::make_shared< RestrictToInterface >( allow_repack_ ) );
					loop_repack_->task_factory( tf_ );
					loop_repack_->apply( pose );
					mc_->boltzmann( pose );
				}


			} // inner
		} // outer
		mc_->recover_low( pose );

		// minimize
		change_FT_to_flankloop_->apply( pose );
		loop_min_mover_->apply( pose );


		closed_cutpoints = cutpoints_separation( pose, ab_info_ );
		++cycle;
	} // while( ( cut_separation > 1.9 )

	TR << "Finished Relaxing CDR H3 Loop" << std::endl;

	return;

}


void H3RefineCCD::set_task_factory(pack::task::TaskFactoryCOP tf) {
	tf_ = utility::pointer::make_shared< pack::task::TaskFactory >(*tf);
}


} // namespace antibody
} // namespace protocols
