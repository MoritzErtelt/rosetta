// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/loops/LoopMover_Perturb_QuickCCD_Moves.cc
/// @brief kinematic loop closure main protocols
/// @author Mike Tyka

//// Unit Headers
#include <protocols/loops/loops_main.hh>
#include <protocols/loops/LoopMover.hh>
#include <protocols/loops/LoopMover_QuickCCD_Moves.hh>
#include <protocols/loops/Loops.hh>
// AUTO-REMOVED #include <protocols/moves/KinematicMover.hh>
#include <protocols/moves/MonteCarlo.hh>
#include <core/conformation/Residue.hh>
//
//
//// Rosetta Headers
// AUTO-REMOVED #include <core/chemical/ChemicalManager.hh>
#include <core/chemical/VariantType.hh>

#include <core/conformation/Conformation.hh>
// AUTO-REMOVED #include <core/conformation/util.hh>
#include <core/conformation/symmetry/SymmetricConformation.hh>
#include <core/id/TorsionID.hh>
#include <core/kinematics/FoldTree.hh>
#include <core/kinematics/MoveMap.hh>
#include <core/optimization/AtomTreeMinimizer.hh>
#include <core/optimization/MinimizerOptions.hh>
#include <basic/options/option.hh>
#include <core/pose/Pose.hh>
// AUTO-REMOVED #include <core/pose/util.hh>
#include <core/scoring/Energies.hh>
// AUTO-REMOVED #include <core/scoring/rms_util.hh>
#include <core/scoring/ScoreFunction.hh>
// AUTO-REMOVED #include <core/scoring/ScoreFunctionFactory.hh>
#include <core/fragment/FragSet.hh>
#include <protocols/basic_moves/FragmentMover.hh>

#include <core/pose/symmetry/util.hh>
#include <core/conformation/symmetry/util.hh>

#include <core/optimization/symmetry/SymAtomTreeMinimizer.hh>

//#include <core/pack/task/TaskFactory.hh>
//#include <core/pack/task/PackerTask.hh>
//#include <core/pack/task/operation/TaskOperations.hh>
// AUTO-REMOVED #include <core/pack/rotamer_trials.hh>
// AUTO-REMOVED #include <core/pack/pack_rotamers.hh>
//#include <protocols/moves/BackboneMover.hh>
#include <core/scoring/constraints/ConstraintSet.hh>
// Auto-header: duplicate removed #include <core/conformation/symmetry/SymmetricConformation.hh>
#include <core/conformation/symmetry/SymmetryInfo.hh>

// AUTO-REMOVED #include <basic/prof.hh> // profiling
#include <basic/Tracer.hh> // tracer output
#include <protocols/loops/ccd_closure.hh>

//Utility Headers
#include <numeric/random/random.hh>
// AUTO-REMOVED #include <utility/io/izstream.hh>

// C++ Headers
#include <iostream>
#include <map>
#include <string>

// option key includes

#include <basic/options/keys/loops.OptionKeys.gen.hh>
#include <basic/options/keys/constraints.OptionKeys.gen.hh>

//Auto Headers
#include <core/pose/util.hh>





namespace protocols {
namespace loops {

///////////////////////////////////////////////////////////////////////////////
using namespace core;
extern basic::Tracer tr;


static numeric::random::RandomGenerator RG(42862);


LoopMover_Perturb_QuickCCD_Moves::LoopMover_Perturb_QuickCCD_Moves(
	protocols::loops::Loops  loops_in
) :
	LoopMover_Perturb_QuickCCD( loops_in )
{
}



LoopMover_Perturb_QuickCCD_Moves::LoopMover_Perturb_QuickCCD_Moves(
	protocols::loops::Loops  loops_in,
	core::scoring::ScoreFunctionOP  scorefxn
) :
	LoopMover_Perturb_QuickCCD( loops_in, scorefxn )
{
}

std::string
LoopMover_Perturb_QuickCCD_Moves::get_name() const {
	return "LoopMover_Perturb_QuickCCD_Moves";
}

LoopResult LoopMover_Perturb_QuickCCD_Moves::model_loop(
	core::pose::Pose & pose,
  protocols::loops::Loop const & loop
){
	using namespace kinematics;
	using namespace scoring;
	using namespace optimization;
	using namespace protocols::basic_moves;
	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	using namespace numeric::random;

	tr << "***** DOING CCD MOVES *****" << std::endl;

	core::Size const nres =  pose.total_residue();

	// store starting fold tree and cut pose
 	kinematics::FoldTree f_orig=pose.fold_tree();
	set_single_loop_fold_tree( pose, loop );

	// generate movemap after fold_tree is set
	kinematics::MoveMapOP mm_one_loop = new kinematics::MoveMap(), mm_one_loop_symm = new kinematics::MoveMap();
	set_move_map_for_centroid_loop( loop, *mm_one_loop );
	set_move_map_for_centroid_loop( loop, *mm_one_loop_symm );
	if ( core::pose::symmetry::is_symmetric( pose ) )  {
		core::pose::symmetry::make_symmetric_movemap( pose, *mm_one_loop_symm );
	}

	int const loop_size( loop.stop() - loop.start() + 1 );
	core::Size cycles2 =  10;
	core::Size cycles3 =  std::max( 30, int( 10*loop_size));
	tr.Info << "Number of cycles: cycles2 and cycles3 " << cycles2 << " " << cycles3 << std::endl;

	// special case ... vrt res at last position
	//chainbreak_present &= ( loop.stop() != nres-1 || pose.residue( nres ).aa() != core::chemical::aa_vrt );
	bool chainbreak_present = ( loop.start() != 1 && loop.stop() != nres &&
	                            !pose.residue( loop.start() ).is_lower_terminus() &&
	                            !pose.residue( loop.stop() ).is_upper_terminus() );

	// set loop.cut() variant residue for chainbreak score if chanbreak is present
	if( chainbreak_present ){
		core::pose::add_variant_type_to_pose_residue( pose, chemical::CUTPOINT_LOWER, loop.cut() );
		core::pose::add_variant_type_to_pose_residue( pose, chemical::CUTPOINT_UPPER, loop.cut()+1 );
	}

	(*scorefxn_)(pose);
	/// Now handled automatically.  scorefxn_->accumulate_residue_total_energies( pose );
	core::pose::Pose start_pose = pose;



	// either extend or at least idealize the loop (just in case).
	if( loop.is_extended() ) set_extended_torsions( pose, loop );
	else                     idealize_loop(  pose, loop );

	// omega needs to be moveable for FragmentMovers, so use a separate movemap
	kinematics::MoveMapOP frag_mover_movemap = new kinematics::MoveMap();
	frag_mover_movemap->set_bb_true_range( loop.start(), loop.stop() );

	std::vector< FragmentMoverOP > fragmover;
	for ( std::vector< core::fragment::FragSetOP >::const_iterator
				it = frag_libs_.begin(), it_end = frag_libs_.end();
				it != it_end; it++ ) {
		ClassicFragmentMover *cfm = new ClassicFragmentMover( *it, frag_mover_movemap );
		cfm->set_check_ss( false );
		cfm->enable_end_bias_check( false );
		fragmover.push_back( cfm );
	}

	core::Real m_Temperature_ = 2.0;
	moves::MonteCarloOP mc_ = new moves::MonteCarlo( pose, *scorefxn_, m_Temperature_ );

	if( randomize_loop_ ){
		// insert random fragment as many times as the loop is long (not quite the exact same as the old code)
		for ( Size i = loop.start(); i <= loop.stop(); ++i ) {
			for ( std::vector< FragmentMoverOP >::const_iterator
						it = fragmover.begin(),it_end = fragmover.end(); it != it_end; it++ ) {
				(*it)->apply( pose );
			}
		}
	}

	// Set up Minimizer object
	AtomTreeMinimizerOP mzr;
	float const dummy_tol( 0.001 ); // linmin sets tol internally
	bool const use_nblist( true ), deriv_check( false ); // true ); // false );
	MinimizerOptions options( "linmin", dummy_tol, use_nblist, deriv_check);
	if ( core::pose::symmetry::is_symmetric( pose ) ) {
		mzr = new core::optimization::symmetry::SymAtomTreeMinimizer;
	} else {
		mzr = new core::optimization::AtomTreeMinimizer;
	}



	// Set up MonteCarlo Object
	core::Real const init_temp = 2.0;
	core::Real temperature = init_temp;
 	mc_->reset( pose );
	mc_->set_temperature( temperature );

	int   frag_count   = 0;
	scorefxn_->show_line_headers( tr.Info );

	float final_chain_break_weight = 5.0;
	if ( core::pose::symmetry::is_symmetric( pose ) ) {
		core::conformation::symmetry::SymmetricConformation const & symm_conf (
					dynamic_cast<core::conformation::symmetry::SymmetricConformation const & > ( pose.conformation()) );
		core::conformation::symmetry::SymmetryInfoCOP symm_info( symm_conf.Symmetry_Info() );
		final_chain_break_weight = 5.0*symm_info->subunits();
	}



	float const delta_weight( final_chain_break_weight/cycles2 );
	scorefxn_->set_weight( chainbreak, 0.0 ); //not evaluating quadratic chainbreak
	mc_->score_function( *scorefxn_ );

	bool 	has_constraints                  = pose.constraint_set()->has_constraints();
	float final_constraint_weight          = option[ basic::options::OptionKeys::constraints::cst_weight ]();
	for ( core::Size c2 = 1; c2 <= cycles2; ++c2 ) {
		mc_->recover_low( pose );

		// ramp up constraints
		if( has_constraints ) {
			if( c2 != cycles2 ) {
				scorefxn_->set_weight( core::scoring::atom_pair_constraint, final_constraint_weight*float(c2)/float( cycles2 ) );
			} else {
				scorefxn_->set_weight( core::scoring::atom_pair_constraint, final_constraint_weight * 0.2 );
			}
		}
		if ( chainbreak_present ) {
			scorefxn_->set_weight( linear_chainbreak, c2*delta_weight );
		}
		mc_->score_function( *scorefxn_ );

		// score and print an info line
		(*scorefxn_)(pose);
		scorefxn_->show_line( tr , pose );
		tr << std::endl;

		for ( core::Size c3 = 1; c3 <= cycles3; ++c3 ) {
			if(( !chainbreak_present || uniform()*cycles2 > c2 ))
			{
				//do fragment moves here
				for ( std::vector< FragmentMoverOP >::const_iterator
							it = fragmover.begin(),it_end = fragmover.end(); it != it_end; it++ ) {


					if( ((*it)->fragments()->max_frag_length() == 1 ) && (uniform() < option[OptionKeys::loops::skip_1mers ]() ) ) continue;
					if( ((*it)->fragments()->max_frag_length() == 3 ) && (uniform() < option[OptionKeys::loops::skip_3mers ]() ) ) continue;
					if( ((*it)->fragments()->max_frag_length() == 9 ) && (uniform() < option[OptionKeys::loops::skip_9mers ]() ) ) continue;

					(*it)->apply( pose );
					frag_count++;
				}
			} else {
				//do ccd_moves here
				if( ! option[OptionKeys::loops::skip_ccd_moves ]() ){
					protocols::loops::ccd_moves(5, pose, *mm_one_loop, loop.start(), loop.stop(), loop.cut() );
				}
			}
			mc_->boltzmann( pose, "QuickCCD_Moves" );
		} // cycles3
		mzr->run( pose, *mm_one_loop_symm, *scorefxn_, options );
	} //cycles2

	// now ensure the loop is properly closed!
	float chain_break_score = std::max( (float)pose.energies().total_energies()[ scoring::chainbreak ],
																(float)pose.energies().total_energies()[ scoring::linear_chainbreak ] );

	if( ( chain_break_score > 0.1 ) && chainbreak_present )
	{
		mc_->recover_low( pose );
		tr << "--" << std::endl;
		(*scorefxn_)(pose);
		scorefxn_->show_line( tr , pose );
		tr << std::endl;
		Loop closeloop(std::max(loop.cut()-3,loop.start()),  std::min(loop.cut()+3,loop.stop()), loop.cut() );
		fast_ccd_close_loops( pose, closeloop, *mm_one_loop );
		(*scorefxn_)(pose);
		scorefxn_->show_line( tr , pose );
		tr << std::endl;
		mc_->reset( pose );
	}

	pose = mc_->lowest_score_pose();
	//scorefxn_->show(  tr.Info , pose );
	tr.Info << "-------------------------" << std::endl;
	mc_->show_counters();

	// CHeck chain break !
	if( chainbreak_present ){
		(*scorefxn_)( pose );
		core::Real chain_break_score = std::max( (float)pose.energies().total_energies()[ scoring::chainbreak ],
																	(float)pose.energies().total_energies()[ scoring::linear_chainbreak ] );

		core::Real chain_break_tol = option[ basic::options::OptionKeys::loops::chain_break_tol ]();
		tr.Info << "Chainbreak: " << chain_break_score << " Max: " << chain_break_tol << std::endl;
		if( chain_break_score > chain_break_tol ) return Failure;
	}

	// return to original fold tree
	pose.fold_tree( f_orig );

	return Success;
}



} // namespace loops
} // namespace protocols
