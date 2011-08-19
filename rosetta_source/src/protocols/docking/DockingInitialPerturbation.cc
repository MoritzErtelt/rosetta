// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file DockingInitialPerturbation.cc
/// @brief initial position functions
/// @detailed
///		This contains the functions that create initial positions for docking
///		You can either randomize partner 1 or partner 2, spin partner 2, or
///		perform a simple perturbation.
/// 	Also contains docking mcm protocol
/// @author Monica Berrondo

#include <protocols/docking/DockingInitialPerturbation.hh>

// Rosetta Headers
#include <protocols/moves/Mover.hh>
#include <protocols/moves/RigidBodyMover.hh>

#include <core/pose/Pose.hh>
#include <core/pose/Pose.fwd.hh>
#include <basic/options/option.hh>
#include <core/scoring/Energies.hh>

#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>


// ObjexxFCL Headers
// AUTO-REMOVED #include <ObjexxFCL/format.hh>
#include <ObjexxFCL/string.functions.hh>

// C++ Headers
#include <string>

//Utility Headers
// AUTO-REMOVED #include <numeric/conversions.hh>

#include <numeric/trig.functions.hh>
#include <numeric/xyzMatrix.fwd.hh>

#include <basic/Tracer.hh>
#include <utility/tools/make_vector1.hh>
using basic::T;

// option key includes

#include <basic/options/keys/docking.OptionKeys.gen.hh>


using basic::Error;
using basic::Warning;

static basic::Tracer TR("protocols.docking.DockingInitialPerturbation");
static core::Size trans ( 1 ), rot ( 2 );

using namespace core;

namespace protocols {
namespace docking {


// initial perturbation on one of the partners
// the type of perturbation is defined in the options
// some of the options are randomize1 (randomizes the first partner)
// randomize2 (randomizes the second partner), dock_pert, and spin
//------------------------------------------------------------------------------
//
//     there are several ways to perturb the structure before beginning
//     the search; they are controlled through command-line flags
//
//     at the end, partners are slid into contact and scored
//


//constructors
DockingInitialPerturbation::DockingInitialPerturbation(): Mover(), slide_( true )
{
	Mover::type( "DockingInitialPerturbation" );
	movable_jumps_ = utility::tools::make_vector1<core::Size>(1);
	multiple_jumps_ = false;
	init();
}

DockingInitialPerturbation::DockingInitialPerturbation(
	core::Size const rb_jump,
	bool const slide
) : Mover(), slide_(slide)
{
	Mover::type( "DockingInitialPerturbation" );
	movable_jumps_ = utility::tools::make_vector1<core::Size>(rb_jump);
	multiple_jumps_ = false;
	init();
}

DockingInitialPerturbation::DockingInitialPerturbation(
	DockJumps const movable_jumps,
	bool const slide
): Mover(), slide_(slide), movable_jumps_(movable_jumps)
{
	Mover::type( "DockingInitialPerturbation" );
	if ( movable_jumps_.size() > 1 ) multiple_jumps_ = true;
	else multiple_jumps_ = false;
	init();
}

void
DockingInitialPerturbation::init()
{
	set_default();
	register_options();
	init_from_options();
}

void
DockingInitialPerturbation::set_default()
{
	randomize1_ = false;
	randomize2_ = false;
	if_dock_pert_ = false;
	if_uniform_trans_ = false;
	spin_ = false;
	center_at_interface_ = false;
//	dock_pert_ = new utility::vector1< Real >(NULL);
//	uniform_trans_ = NULL;
}


//destructor
DockingInitialPerturbation::~DockingInitialPerturbation() {}

// ALERT!
// register_options() and init_from_options() are not called anywhere yet!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!11

void
DockingInitialPerturbation::init_from_options()
{
	using namespace basic::options;
	TR << "Reading options..." << std::endl;

	if ( option[ OptionKeys::docking::randomize1 ].user() )
		set_randomize1(option[ OptionKeys::docking::randomize1 ]());

	if ( option[ OptionKeys::docking::randomize2 ].user() )
		set_randomize2(option[ OptionKeys::docking::randomize2 ]());

	if ( option[ OptionKeys::docking::dock_pert ].user() )
		set_dock_pert(option[ OptionKeys::docking::dock_pert ]());


	if ( option[ OptionKeys::docking::uniform_trans ].user() )
		set_uniform_trans(option[ OptionKeys::docking::uniform_trans ]());

	if ( option[ OptionKeys::docking::spin ].user() )
		set_spin(option[ OptionKeys::docking::spin ]());

	if ( option[ OptionKeys::docking::center_at_interface ].user() )
		set_center(option[ OptionKeys::docking::center_at_interface ]());
}

void
DockingInitialPerturbation::register_options()
{
	using namespace basic::options;

	option.add_relevant( OptionKeys::docking::randomize1 );
	option.add_relevant( OptionKeys::docking::randomize2 );
	option.add_relevant( OptionKeys::docking::dock_pert );
	option.add_relevant( OptionKeys::docking::uniform_trans );
	option.add_relevant( OptionKeys::docking::spin );
	option.add_relevant( OptionKeys::docking::center_at_interface );
}


////////////////////////////////////////////////////////////////////////////////
/// @begin initial_perturbation
///
/// @brief   Make starting perturbations for rigid body moves
///
/// @detailed    There are several ways to perturb the structure before beginning
///     the search; they are controlled through command-line flags
///     At the end, partners are slid into contact and scored (including
///     mc_reset).
///     Also, they are tested for passing the FAB filter.
///
///
/// @references see dock_structure or pose_docking from rosetta++
///
/// @authors Monica Berrondo June 14 2007
///
/// @last_modified October 17 2007
/////////////////////////////////////////////////////////////////////////////////
void DockingInitialPerturbation::apply( core::pose::Pose & pose )
{
	for ( DockJumps::const_iterator it=movable_jumps_.begin(); it != movable_jumps_.end(); ++it){
		apply_body(pose, *it);
	}
}

void
DockingInitialPerturbation::apply_body(core::pose::Pose & pose, core::Size jump_number )
{
	using namespace moves;

	if( randomize1_ ) {
		TR << "randomize1: true" << std::endl;
		RigidBodyRandomizeMover mover( pose, jump_number, partner_upstream );
		mover.apply( pose );
	}
	if( randomize2_ ) {
		TR << "randomize2: true" << std::endl;
		RigidBodyRandomizeMover mover( pose, jump_number, partner_downstream );
		mover.apply( pose );
	}
	if( if_dock_pert_ ) {
		// DO NOT supply default values for this option -- reasonable values differ for protein and ligand protocols.
		// Also, default values will cause a perturbation to *always* occur, even with no command line flags -- very surprising.
		// Adding defaults WILL BREAK existing protocols in unexpected ways.
		// Decided by Jeff, Monica, Ian, and Sid in March 2008.
		//
		// Jeff notes that eventually there should be 3 parameters, like Rosetta++:
		// rotation, normal translation, and perpendicular translation.
		TR << "dock_pert: true" << std::endl;
		/// read in dock_pert options from commandline.  the first value is the
		/// rotation magnitude and the second value is the translational value
		utility::vector1< Real > pert_mags = dock_pert_;
		//TR << "option[ docking::rotational ]()" << rot << "\n";
		//TR << "option[ docking::parallel ]()" << trans << "\n";
		TR << "option[ docking::dock_pert ]()" << pert_mags[rot] << ' ' << pert_mags[trans] << std::endl;
		RigidBodyPerturbMoverOP mover;
		if (center_at_interface_) mover = new RigidBodyPerturbMover( jump_number, pert_mags[rot], pert_mags[trans], partner_downstream, true );
		else mover = new RigidBodyPerturbMover( jump_number, pert_mags[rot], pert_mags[trans] );
		mover->apply( pose );
	}

	if( if_uniform_trans_ ) {
		core::Real mag( uniform_trans_ );
		TR << "uniform_trans: " << mag << std::endl;
		UniformSphereTransMover mover( jump_number, mag );
		mover.apply( pose );
	}
	if( spin_ ) {
		TR << "axis_spin: true" << std::endl;
		RigidBodySpinMover mover( jump_number );
		mover.apply( pose );
	}
	// DO NOT do this for e.g. ligand docking
	if ( slide_ ) {
		DockingSlideIntoContact slide( jump_number );
		slide.apply( pose );
	}

}
std::string
DockingInitialPerturbation::get_name() const {
	return "DockingInitialPerturbation";
}

////////////////////////////////////////// DockingSlideIntoContact ////////////////////////////////

// default constructor
DockingSlideIntoContact::DockingSlideIntoContact() : Mover()
{
	using namespace core::scoring;
	Mover::type( "DockingSlideIntoContact" );
	rb_jump_ = 1;
	scorefxn_ = ScoreFunctionFactory::create_score_function( CENTROID_WTS, DOCK_LOW_PATCH );
	scorefxn_ = ScoreFunctionFactory::create_score_function( "interchain_cen" );
}

//constructor
DockingSlideIntoContact::DockingSlideIntoContact(
	core::Size const rb_jump
) : Mover(), rb_jump_(rb_jump)
{
	using namespace core::scoring;
	Mover::type( "DockingSlideIntoContact" );
	scorefxn_ = ScoreFunctionFactory::create_score_function( CENTROID_WTS, DOCK_LOW_PATCH );
	scorefxn_ = ScoreFunctionFactory::create_score_function( "interchain_cen" );
}

//destructor
DockingSlideIntoContact::~DockingSlideIntoContact() {}


void DockingSlideIntoContact::apply( core::pose::Pose & pose )
{
	using namespace moves;

	RigidBodyTransMover mover( pose, rb_jump_ );
	( *scorefxn_ )( pose );

	TR << "sliding into contact" << std::endl;
	TR << "Moving away" << std::endl;
	core::Size const counter_breakpoint( 500 );
	core::Size counter( 0 );
	// first try moving away from each other
	while ( pose.energies().total_energies()[ scoring::interchain_vdw ] > 0.1 && counter <= counter_breakpoint ) {
		mover.apply( pose );
		( *scorefxn_ )( pose );
		++counter;
	}
	if( counter > counter_breakpoint ){
		TR<<"failed moving away with original vector. Aborting DockingSlideIntoContact."<<std::endl;
		set_current_tag( "fail" );
		return;
	}
	counter = 0;
	// then try moving towards each other
	TR << "Moving together" << std::endl;
	mover.trans_axis().negate();
	while ( counter <= counter_breakpoint && pose.energies().total_energies()[ scoring::interchain_vdw ] < 0.1 ) {
		mover.apply( pose );
		( *scorefxn_ )( pose );
		++counter;
	}
	if( counter > counter_breakpoint ){
		TR<<"moving together failed. Aborting DockingSlideIntoContact."<<std::endl;
		set_current_tag( "fail" );
		return;
	}
	// move away again until just touching
	mover.trans_axis().negate();
	mover.apply( pose );
}

std::string
DockingSlideIntoContact::get_name() const {
	return "DockingSlideIntoContact";
}

////////////////////////////////////////// FaDockingSlideIntoContact ////////////////////////////////

// default constructor
FaDockingSlideIntoContact::FaDockingSlideIntoContact()
{

	Mover::type( "FaDockingSlideIntoContact" );
	rb_jump_ = 1;
	scorefxn_ = new core::scoring::ScoreFunction();
	scorefxn_->set_weight( core::scoring::fa_rep, 1.0 );
}


//constructor
FaDockingSlideIntoContact::FaDockingSlideIntoContact(
	core::Size const rb_jump
) : Mover(), rb_jump_(rb_jump), tolerance_(0.2)
{
	Mover::type( "FaDockingSlideIntoContact" );
	scorefxn_ = new core::scoring::ScoreFunction();
	scorefxn_->set_weight( core::scoring::fa_rep, 1.0 );
}

//destructor
FaDockingSlideIntoContact::~FaDockingSlideIntoContact() {}

void FaDockingSlideIntoContact::apply( core::pose::Pose & pose )
{
	using namespace core::scoring;

	// A very hacky way of guessing whether the components are touching:
	// if pushed together by 1A, does fa_rep change at all?
	// (The docking rb_* score terms aren't implemented as of this writing.)
	(*scorefxn_)( pose );
	core::Real const initial_fa_rep = pose.energies().total_energies()[ fa_rep ];
	bool are_touching = false;
	moves::RigidBodyTransMover trans_mover( pose, rb_jump_ );

	//int i=1;
	// Take 2A steps till clash, then back apart one step.  Now you're within 2A of touching.
	// Repeat with 1A steps, 0.5A steps, 0.25A steps, etc until you're as close are you want.
	for( core::Real stepsize = 2.0; stepsize > tolerance_; stepsize /= 2.0 ) {
		trans_mover.trans_axis( trans_mover.trans_axis().negate() ); // now move together
		trans_mover.step_size(stepsize);
		core::Size const counter_breakpoint( 500 );
		core::Size counter( 0 );
		do
		{
			trans_mover.apply( pose );
			(*scorefxn_)( pose );
			core::Real const push_together_fa_rep = pose.energies().total_energies()[ fa_rep ];
			//std::cout << "fa_rep = " << push_together_fa_rep << std::endl;
			are_touching = (std::abs(initial_fa_rep - push_together_fa_rep) > 1e-4);
			//std::ostringstream s;
			//s << "snapshot" << i << ".pdb";
			//pose.dump_pdb(s.str());
			//i += 1;
			++counter;
		} while( counter <= counter_breakpoint && !are_touching );
		if( counter > counter_breakpoint ){
			TR<<"Failed Fadocking Slide Together. Aborting."<<std::endl;
			set_current_tag( "fail" );
		}
		trans_mover.trans_axis( trans_mover.trans_axis().negate() ); // now move apart
		trans_mover.apply( pose );
	}
}

std::string
FaDockingSlideIntoContact::get_name() const {
	return "FaDockingSlideTogether";
}

} // namespace docking
} // namespace protocols
