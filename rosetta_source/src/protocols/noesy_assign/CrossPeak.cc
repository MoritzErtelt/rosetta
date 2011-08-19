// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file FragmentSampler.cc
/// @brief ab-initio fragment assembly protocol for proteins
/// @detailed
///	  Contains currently: Classic Abinitio
///
///
/// @author Oliver Lange

// Unit Headers
#include <protocols/noesy_assign/CrossPeak.hh>
#include <protocols/noesy_assign/ResonanceList.hh>

// Package Headers
#include <protocols/noesy_assign/Exceptions.hh>
#include <protocols/noesy_assign/PeakAssignmentParameters.hh>

// Project Headers
#include <core/chemical/AA.hh>
#include <core/scoring/constraints/AmbiguousNMRConstraint.hh>
#include <core/scoring/constraints/AmbiguousNMRDistanceConstraint.hh>
#include <core/scoring/constraints/BoundConstraint.hh>
// Utility headers
#include <ObjexxFCL/format.hh>
#include <ObjexxFCL/string.functions.hh>

#include <utility/string_util.hh>
#include <utility/exit.hh>
// #include <utility/vector1.fwd.hh>
// #include <utility/pointer/ReferenceCount.hh>
// #include <numeric/numeric.functions.hh>
#include <basic/prof.hh>
#include <basic/Tracer.hh>
// #include <core/options/option.hh>
// #include <core/options/keys/abinitio.OptionKeys.gen.hh>
// #include <core/options/keys/run.OptionKeys.gen.hh>
//#include <core/options/keys/templates.OptionKeys.gen.hh>

//// C++ headers
#include <iostream>
#include <cstdlib>
#include <string>




static basic::Tracer tr("protocols.noesy_assign.crosspeaks");
static basic::Tracer tr_labels("protocols.noesy_assign.crosspeaks.labels");

using core::Real;
using namespace core;
using namespace basic;


namespace protocols {
namespace noesy_assign {

std::set< core::id::NamedAtomID > CrossPeak::unknown_resonances_;

CrossPeak::Spin::Spin( Real freq ) : freq_ ( freq ) {}
CrossPeak::Spin::Spin() { }
CrossPeak::Spin::~Spin() {}
core::Size CrossPeak::Spin::assignment_index( core::Size assignment ) const {
	core::Size ct( 1 );
	for ( SpinAssignments::const_iterator it = assignments_.begin(); it != assignments_.end(); ++it ) {
		if ( assignment == *it ) return ct;
		++ct;
	}
	return 0;
}

CrossPeak::CrossPeak( Spin const& sp1, Spin const& sp2, Real strength ) :
  proton1_( sp1 ),
  proton2_( sp2 ),
  volume_( strength ),
	cumulative_peak_volume_( 1.0 ),
	distance_bound_( 100 ), //what would be a good initial value? -1 ?
	eliminated_( NOT_ELIMINATED ),
	eliminated_due_to_dist_violations_( false )
{}

CrossPeak::CrossPeak() :
	cumulative_peak_volume_( 1.0 ),
	distance_bound_( 100 ),
	eliminated_( NOT_ELIMINATED ),
	eliminated_due_to_dist_violations_( false )
{}

CrossPeak::~CrossPeak() {}

///@detail use for reading of assignments from file
/// pass as spin1, spin2, label1, label2 (indices 1..4)
void CrossPeak::add_full_assignment( Size res_ids[] ) {
// 	std::cerr << "CrossPeak::add_full_assignment stubbed out " << res_ids[ 1 ] << std::endl;
//  	std::cerr << "run with 'ignore_assignments'" << std::endl;
// 	utility_exit_with_message( "stubbed function" );
	Size ind1 = assign_spin( 1, res_ids );
	Size ind2 = assign_spin( 2, res_ids );
	assignments_.push_back( new PeakAssignment( this, ind1, ind2 ) );

	//	for ( Size i = 1;
	//need to find resonances in Spins and add them if they are still missing.
	//get id for spin1 and spin2
	//then make PeakAssignment() and add to assignments_
}


///@brief find all possible assignments based on chemical shifts and tolerances
void CrossPeak::find_assignments( ) {

	if ( proton1_.n_assigned() && proton2_.n_assigned() ) return; //if assignments are already present do nothing

  runtime_assert( proton1_.n_assigned() == proton2_.n_assigned() );
  assign_spin( 1 );
  assign_spin( 2 );

  Size const n_assigned_1( proton( 1 ).n_assigned() );
  Size const n_assigned_2( proton( 2 ).n_assigned() );

  for ( Size ct1 = 1; ct1 <= n_assigned_1; ++ct1 ) {
    for ( Size ct2 = 1; ct2 <= n_assigned_2; ++ct2 ) {
      assignments_.push_back( new PeakAssignment( this, ct1, ct2 ) );
    }
  }
  //write_to_stream( tr.Debug );
  //  tr.Debug << std::endl;
}

void CrossPeak::print_peak_info( std::ostream& os ) const {
	os << "peak: " << peak_id() << " ";
	for ( Size i = 1; i<=2 ; i++ ) {
		os << info_struct( i ).main_atom();
		if ( info_struct( i ).has_label() ) os << "-" << info_struct( i ).label_atom_type();
		if ( i == 1 ) os << " <-> ";
	}
}

///@brief assign protons based on chemical shifts and tolerances
void CrossPeak::assign_spin( Size iproton ) {
  //base-class: disregard label
  Real const my_freq( proton( iproton ).freq() );
  Real const my_tolerance( info_struct( iproton ).proton_tolerance() );
  for ( ResonanceList::const_iterator it = resonances().begin(); it != resonances().end(); ++it )  {
    if ( std::abs( it->second.freq() - my_freq ) < std::max( my_tolerance, it->second.error() ) ) {
      proton( iproton ).add_assignment( it->first );
    }
  }
}

///@brief assign protons ass pre-determined
core::Size CrossPeak::assign_spin( Size iproton, Size res_id[] ) {
	Size ind = proton( iproton ).assignment_index( res_id[ iproton ] );
	if ( ind ) return ind;
	proton( iproton ).add_assignment( res_id[ iproton ] );
	return proton( iproton ).n_assigned();
}

void
CrossPeak::create_fa_and_cen_constraint(
			core::scoring::constraints::ConstraintOP& fa_cst,
			core::scoring::constraints::ConstraintOP& cen_cst,
			pose::Pose const& pose,
			pose::Pose const& centroid_pose,
			core::Size normalization,
			bool fa_only
) const {
	using namespace core::scoring::constraints;
	PeakAssignmentParameters const& params( *PeakAssignmentParameters::get_instance() );
	core::Real weight( sqrt( 1.0*normalization )/params.cst_strength_ );

	FuncOP func( new BoundFunc(1.5,
				distance_bound(),
				weight,
				"automatic NOE Peak "+ObjexxFCL::string_of( peak_id() )+" "+filename()
			)
		);

	Size ct_ambiguous( 0 );
	PeakAssignments::const_iterator first_valid = end();
	for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
		if ( (*it)->normalized_peak_volume() <= params.min_volume_ ) continue; //not enough contribution
		if ( !ct_ambiguous ) first_valid = it;
		++ct_ambiguous;
	}

	if ( ct_ambiguous > 1 ) {
		/// create Ambiguous constraint with BoundFunc describing the whole thing...
		AmbiguousNMRConstraintOP
			my_fa_cst = new AmbiguousNMRConstraint( func );

		AmbiguousNMRConstraintOP
			my_cen_cst = new AmbiguousNMRConstraint( func );

		core::Size n( n_assigned() );
		//mjo commenting out 'sd' because it is unused and causes a warning
		//core::Real sd( 1.0/params.cst_strength_ );
		core::Real eff_single_dist( pow( pow( distance_bound(), -6 ) / n, -1.0/6 ) ); //assuming equal contribution of all which is of course wrong
		core::Real cum_new_distance( 0 );

		// add individual constraints --- their potential does not matter ... it is ignored in evaluation...
		for ( PeakAssignments::const_iterator it = first_valid; it != end(); ++it ) {
			if ( (*it)->normalized_peak_volume() <= params.min_volume_ ) continue; //not enough contribution
			AmbiguousNMRDistanceConstraintOP new_cst( (*it)->create_constraint( pose ) );
			my_fa_cst->add_individual_constraint( new_cst );
			if ( !fa_only ) {
				Size number_of_maps; //are 0, 1 or 2 sidechains replaced by CB
				my_cen_cst->add_individual_constraint( new_cst->map_to_CB( pose, centroid_pose, number_of_maps ) );
				cum_new_distance += pow( (eff_single_dist + params.centroid_mapping_distance_padding_*number_of_maps )*pow( new_cst->multiplicity(), 1.0/6 ), -6 );
			}
		}
		fa_cst = my_fa_cst;
		if ( !fa_only ) {
			cen_cst = my_cen_cst->clone( new BoundFunc( 1.5,
					pow( cum_new_distance, -1.0/6 ),
					weight,
					"CB mapped automatic NOE: Peak "+ ObjexxFCL::string_of( peak_id() )
				) );
		}
	} else { // not ambiguous
		if ( first_valid == end() ) return;
		AmbiguousNMRDistanceConstraintOP my_fa_cst = (*first_valid)->create_constraint( pose, func ); //first one should be only one,
		fa_cst = my_fa_cst;
		if ( !fa_only ) {
			Size number_of_maps; //are 0, 1 or 2 sidechains replaced by CB
			core::Size n( n_assigned() );
			core::Real eff_single_dist( pow( pow( distance_bound(), -6 ) / n, -1.0/6 ) ); //assuming equal contribution of all which is of course wrong
			core::Real cum_new_distance( 0 );

			ConstraintOP my_cen_cst = my_fa_cst->map_to_CB( pose, centroid_pose, number_of_maps );
			cum_new_distance += pow( (eff_single_dist + params.centroid_mapping_distance_padding_*number_of_maps )*pow( my_fa_cst->multiplicity(), 1.0/6 ), -6 );

			cen_cst = my_cen_cst->clone( new
				BoundFunc( 1.5,
					pow( cum_new_distance, -1.0/6 ),
					weight,
					"CB mapped automatic NOE: Peak "+ ObjexxFCL::string_of( peak_id() )
				) );
		}
		//	tr.Trace << "constraint for " << peak_id() << " finished " << std::endl;
	}
}

core::scoring::constraints::ConstraintOP
CrossPeak::create_constraint( pose::Pose const& pose, core::Size normalization ) const {


	basic::ProfileThis doit( basic::NOESY_ASSIGN_CP_GEN_CST );

	PeakAssignmentParameters const& params( *PeakAssignmentParameters::get_instance() );
	core::Real weight( sqrt( 1.0*normalization )/params.cst_strength_ );

	core::scoring::constraints::FuncOP func( new core::scoring::constraints::BoundFunc( 1.5, distance_bound(), weight, "automatic NOE Peak "+ObjexxFCL::string_of( peak_id() )+" "+filename() ) );

	Size ct_ambiguous( 0 );
	for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
		if ( (*it)->normalized_peak_volume() <= params.min_volume_ ) continue; //not enough contribution
		++ct_ambiguous;
	}

	if ( ct_ambiguous > 1 ) {
		core::scoring::constraints::AmbiguousNMRConstraintOP
			constraint = new core::scoring::constraints::AmbiguousNMRConstraint( func );

		for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
			if ( (*it)->normalized_peak_volume() <= params.min_volume_ ) continue; //not enough contribution
			constraint->add_individual_constraint( (*it)->create_constraint( pose ) );
		}
		return constraint;
	} else {
		for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
			if ( (*it)->normalized_peak_volume() <= params.min_volume_ ) continue; //not enough contribution
			return (*it)->create_constraint( pose, func ); //first one should be only one,
		}
	}
	return NULL; //never reached
}

core::scoring::constraints::ConstraintOP
CrossPeak::create_centroid_constraint(
	pose::Pose const& pose,
	pose::Pose const& centroid_pose,
	core::Size normalization
 ) const {
	using namespace core::scoring::constraints;


	PeakAssignmentParameters const& params( *PeakAssignmentParameters::get_instance() );
	core::Real weight( sqrt( 1.0*normalization )/params.cst_strength_ );

	//	if ( ct_ambiguous>1 ) {
		AmbiguousNMRConstraintOP constraint =
			new AmbiguousNMRConstraint
			( new BoundFunc( 1.5, distance_bound(), weight, "automatic NOE Peak "+ObjexxFCL::string_of( peak_id() ) ) );
		core::Size n( n_assigned() );
		core::Real sd( 1.0/params.cst_strength_ );
		core::Real eff_single_dist( pow( pow( distance_bound(), -6 ) / n, -1.0/6 ) ); //assuming equal contribution of all which is of course wrong
		core::Real cum_new_distance( 0 );
		for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
			if ( (*it)->normalized_peak_volume() <= params.min_volume_ ) continue; //not enough contribution
			AmbiguousNMRDistanceConstraintOP new_cst( (*it)->create_constraint( pose ) );
			Size number_of_maps; //are 0, 1 or 2 sidechains replaced by CB
			constraint->add_individual_constraint( new_cst->map_to_CB( pose, centroid_pose, number_of_maps ) );
			cum_new_distance += pow( (eff_single_dist + params.centroid_mapping_distance_padding_*number_of_maps )*pow( new_cst->multiplicity(), 1.0/6 ), -6 );
			sd += params.centroid_mapping_distance_padding_*( pow( new_cst->multiplicity()-1, 1.0/6 ) );
		}
		return constraint->clone( new BoundFunc( 1.5, pow( cum_new_distance, -1.0/6 ), weight, "CB mapped automatic NOE: Peak "+ ObjexxFCL::string_of( peak_id() ) ) );
		/// 10/22/2010 replace sd*weight with weight... centroid csts were generally underweighted compared to fullatom...
// 	} else { //not ambiguous
// 	}
}

///@brief do we have a inter residue assignment with at least volume_threshold contribution ?
Size CrossPeak::min_seq_separation_residue_assignment( Real volume_threshold ) const {
	Size min_seq( 99999 );
	for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
		if ( (*it)->normalized_peak_volume() <= volume_threshold ) continue; //not enough contribution
		Size res1( (*it)->resid( 1 ) );
		Size res2( (*it)->resid( 2 ) );
		Size diff( res1 < res2 ? res2-res1 : res1-res2 );
		min_seq = min_seq < diff ? min_seq : diff;
	}
	return min_seq;
}

std::string const& CrossPeak::elimination_reason() const {
	static std::string const str_distviol( "DistViol" );
	static std::string const str_network( "Network" );
	static std::string const str_minvol( "MinPeakVol");
	static std::string const str_maxassign( "MaxAssign");
	static std::string const empty_str( "" );
	if ( eliminated_ == EL_DISTVIOL ) return str_distviol;
	if ( eliminated_ == EL_NETWORK ) return str_network;
	if ( eliminated_ == EL_MINPEAKVOL ) return str_minvol;
	if ( eliminated_ == EL_MAXASSIGN ) return str_maxassign;
	return empty_str;
}

bool CrossPeak::eliminated( bool recompute, bool do_not_compute ) const {
	if ( recompute && !do_not_compute ) eliminated_ = eliminated_due_to_dist_violations_ ? EL_DISTVIOL : NOT_ELIMINATED;
	//if dist_cut problem eliminated_ should already be set to false.
	//	tr.Trace << "elimination check for peak " << peak_id() << "...";
	//	if ( eliminated_ ) tr.Trace << "eliminated from cached value" << std::endl;
	if ( eliminated_ ) return true;
	if ( do_not_compute ) return false; //not eliminated as of now...

	PeakAssignmentParameters const& params( *PeakAssignmentParameters::get_instance() );
	bool min_peak_volume_reached( false );
	for ( PeakAssignments::const_iterator it = begin(); it != end() && !min_peak_volume_reached ; ++it ) {
		min_peak_volume_reached = (*it)->normalized_peak_volume() > params.min_volume_;
	}

	eliminated_ = !min_peak_volume_reached ? EL_MINPEAKVOL : eliminated_;
	//	if ( eliminated_ ) tr.Trace << "eliminated (min_peak_volume)" << std::endl;
	if ( eliminated_ ) return true;

	bool const too_many_assignments( assignments().size() > params.nmax_ );
	eliminated_ = too_many_assignments ? EL_MAXASSIGN : eliminated_;
	//	if ( eliminated_ ) tr.Trace << "eliminated (too many assignments)" << std::endl;
	if ( eliminated_ ) return true;

	//network anchoring
	Real N_atom_sum( 0.0 );
	Real N_res_sum( 0.0 );
	for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
		Real vol( (*it)->normalized_peak_volume() );
		N_atom_sum +=  vol * (*it)->network_anchoring();
		N_res_sum += vol * (*it)->network_anchoring_per_residue();
	}
	bool pass_network_test( N_res_sum > params.network_reswise_high_ );
	if ( !pass_network_test ) pass_network_test = N_res_sum >= params.network_reswise_min_ && N_atom_sum >= params.network_atom_min_;
	eliminated_ = !pass_network_test ? EL_NETWORK : eliminated_;
	//	if ( eliminated_ ) tr.Trace << "eliminated (network)" << std::endl;

	if ( eliminated_ ) return true;
	//	tr.Trace << "passed" << std::endl;
	return false;
}

core::Size CrossPeak::n_Vmin_assignments() {
	PeakAssignmentParameters const& params( *PeakAssignmentParameters::get_instance() );
	Size ct( 0 );
	for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
		Real vol( (*it)->normalized_peak_volume() );
		ct += vol > params.min_volume_;
	}
	return ct;
}

void CrossPeak::calibrate( PeakCalibrator const& calibrator, PeakCalibrator::TypeCumulator& calibration_types ) {
	PeakAssignmentParameters const& params( *PeakAssignmentParameters::get_instance() );
	Real sum( 0.0 );
	Size ct( 0 );
	if ( volume_ <= 0.0 ) throw utility::excn::EXCN_BadInput("Peak intensity negative or zero for "+ObjexxFCL::string_of( peak_id_ ) );
	for ( PeakAssignments::const_iterator it = begin(); it != end(); ++it ) {
		CALIBRATION_ATOM_TYPE type1, type2;
		type1 = (*it)->calibration_atom_type( 1 );
		type2 = (*it)->calibration_atom_type( 2 );
		calibration_types.set( type1 );
		calibration_types.set( type2 );
		Real const cal( sqrt( calibrator( type1 ) * calibrator( type2 ) ) );
		Real vol( (*it)->normalized_peak_volume() );
		sum += (vol > params.min_volume_ ? vol : 0 ) / cal;
		ct += vol > params.min_volume_;
	}
	if ( ct > 0 )	distance_bound_ = pow( sum*volume_, -1.0/6.0 );
	else distance_bound_ = 0.0;
}

///@brief assign protons ass pre-determined
Size CrossPeak3D::assign_spin( Size iproton, Size res_id[] ) {
	Size ind = CrossPeak::assign_spin( iproton, res_id );
	if ( iproton == 1  && ind > label( iproton ).n_assigned() ) label( iproton ).add_assignment( res_id[ iproton+2 ] );
	return ind;
}

void CrossPeak3D::assign_spin( Size iproton ) {
  if ( iproton == 2 ) CrossPeak::assign_spin( iproton ); //base-class: no label
  else assign_labelled_spin( iproton );
}

void CrossPeak3D::assign_labelled_spin( Size iproton ) {
  runtime_assert( has_label( iproton ));
  Real const my_freq( proton( iproton ).freq() );
  Real const my_label_freq( label( iproton ).freq() );
  Real const my_tolerance( info_struct( iproton ).proton_tolerance() );
  Real const my_label_tolerance( info_struct( iproton ).label_tolerance() );


	/// if we have pseudo 4D spectrum we speed things up a bit by filtering out non-protons here
	if ( my_tolerance > 99 ) {
		for ( ResonanceList::const_iterator it = resonances().begin(); it != resonances().end(); ++it )  {
			if ( std::abs( it->second.freq() - my_label_freq ) < std::max( my_label_tolerance, it->second.error() ) ) {
				//now find all proton-resonances that are bound to this label atom
				core::Size resid( it->second.resid() );
				std::string const& label_name( it->second.name() );
				ResonanceList::Resonances const& residue_list( resonances().resonances_at_residue( resid ) );
				for ( ResonanceList::Resonances::const_iterator rit = residue_list.begin(); rit != residue_list.end(); ++rit ) {
					if ( rit->name()[ 0 ]=='Q' || rit->name().find("H") != std::string::npos ) {
						//						tr.Debug << "resid: " << resid<< " test label: " << label_name << " with proton " << rit->name() << std::endl;
						try {
							std::string possible_label( info_struct( iproton ).
								label_atom_name( rit->name(), resonances().aa_from_resid( resid ) ) );
							//tr.Debug << "found possible label " << possible_label << std::endl;
							if ( possible_label == label_name ) {
								label( iproton ).add_assignment( it->first );
								proton( iproton ).add_assignment( rit->label() );
								//we have found a proton that can be attached to our label
							}
						} catch ( EXCN_UnknownAtomname& exception ) {
							continue;
						}
					} // if rit is proton
				} // for rit
			} // if matched resonance
		}// all resonances
	} else {
		for ( ResonanceList::const_iterator it = resonances().begin(); it != resonances().end(); ++it )  {

			/// if we have pseudo 4D spectrum we speed things up a bit by filtering out non-protons here
			//			if ( my_tolerance > 99 && ( it->second.freq() > 13.0 && info_struct( iproton ).main_atom() == "H" ) ) continue;

			if ( std::abs( it->second.freq() - my_freq ) < std::max( my_tolerance, it->second.tolerance() ) ) {
				Size resid( it->second.atom().rsd() );
				//maybe also map resonance by resid?
				try {
					id::NamedAtomID atomID( info_struct( iproton ).label_atom_name( it->second.atom().atom(), resonances().aa_from_resid( resid ) ), resid );
					Resonance const& label_reso ( resonances()[ atomID ] );
					if ( std::abs( label_reso.freq() - my_label_freq ) < my_label_tolerance ) {
						proton( iproton ).add_assignment( it->first );
						label( iproton ).add_assignment( label_reso.label() );
					}
				} catch ( EXCN_UnknownResonance& exception ) {
					if ( !unknown_resonances_.count( exception.atom() ) ) {
						unknown_resonances_.insert( exception.atom() );
						exception.show( tr.Warning );
						tr.Warning << " as label for atom " << it->second.atom().atom() << " " <<  resonances().aa_from_resid( resid ) << std::endl;
					//				if ( tr.Debug.visible() ) exception.show( tr.Debug );
					}
					continue; //if no label is known we don't assign this proton
				} catch ( EXCN_UnknownAtomname& exception ) { //this happens if we try to assign a proton that can't have a label: i.e., a H in a HCH spectrum
					//tr_labels.Trace << "cannot find label atom for resid: " + it->second.atom().atom() + " " + ObjexxFCL::string_of( resid ) + " --- ignore proton assignment" << std::endl;
					//if ( tr.Debug.visible() ) exception.show( tr.Debug );
					continue;
				}
			}
		}
	}
}
// void CrossPeak::invalidate_assignment( Size iassignment ) {
//   proton( 1 ).invalidate_assignment_by_nr( iassignment );
//   proton( 2 ).invalidate_assignment_by_nr( iassignment );
//   if ( has_label( 1 ) ) label( 1 ).invalidate_assignment_by_nr( iassignment );
//   if ( has_label( 2 ) ) label( 2 ).invalidate_assignment_by_nr( iassignment );
// }

CrossPeak3D::CrossPeak3D( Spin const& sp1, Spin const& sp2, Spin const& label1, Real strength ) :
  CrossPeak( sp1, sp2, strength ),
  label1_( label1 )
{}

CrossPeak3D::CrossPeak3D() {}
CrossPeak3D::~CrossPeak3D() {}

CrossPeak4D::CrossPeak4D( Spin const& sp1, Spin const& sp2, Spin const& label1, Spin const& label2, Real strength ) :
  CrossPeak3D( sp1, sp2, label1, strength ),
  label2_( label2 )
{}

CrossPeak4D::CrossPeak4D() {}
CrossPeak4D::~CrossPeak4D() {}

void CrossPeak4D::assign_spin( Size iproton ) {
  assign_labelled_spin( iproton );
}

Size CrossPeak4D::assign_spin( Size iproton, Size res_id[] ) {
	Size ind = CrossPeak::assign_spin( iproton, res_id );
	if ( ind > label( iproton ).n_assigned() ) label( iproton ).add_assignment( res_id[ iproton+2 ] );
	return ind;
}

// void CrossPeak::read_from_stream( std::istream& is ) {
//   Real freq;
//   is >> freq;
//   proton1_ = Spin( freq );
//   if ( has_label( 1 ) ) {
//     is >> freq;
//     label( 1 ) = Spin( freq );
//   }

//   is >> freq;
//   proton2_ = Spin( freq );
//   if ( has_label( 2 ) ) {
//     is >> freq;
//     label( 2 ) = Spin( freq );
//   }

//   Size number;
//   char letter;
//   is >> number >> letter; //no idea what these mean ...it reads now 3 U or 4 U
//   is >> volume_;
//   Real error_of_strength; //... ???
//   is >> error_of_strength;

//   char letter_e;
//   Size number_0;
//   is >> letter_e;
//   is >> number_0;
//   add_assignment_from_stream( is );
// }

// void CrossPeak::add_assignment_from_stream( std::istream& is ) {
//   Size id;
//   is >> id;
//   if ( id )  proton1_.add_assignment( id );
//   if ( has_label( 1 ) ) {
//     is >> id;
//     if ( id ) label( 1 ).add_assignment( id );
//   }

//   is >> id;
//   if ( id ) proton2_.add_assignment( id );
//   if ( has_label( 2 ) ) {
//     is >> id;
//     if ( id ) label( 2 ).add_assignment( id );
//   }
// }


// void CrossPeak::write_to_stream( std::ostream& os ) const {
//   os << ObjexxFCL::fmt::F( 8, 3, proton1_.freq() ) << " ";
//   if ( has_label( 1 ) ) {
//     os << ObjexxFCL::fmt::F( 8, 3, label( 1 ).freq() ) << " ";
//   }

//   os << ObjexxFCL::fmt::F( 8, 3, proton2_.freq() ) << " ";
//   if ( has_label( 2 ) ) {
//     os << ObjexxFCL::fmt::F( 8, 3, label( 2 ).freq() ) << " ";
//   }

//   os << ObjexxFCL::fmt::E( 10, 3, strength_ ) << " " << ObjexxFCL::fmt::E( 10, 3, 0.0 ) << " ";

//   Size assignments_written( 0 );
//   //  while ( assignments_written < proton1_.n_assigned() ) {
//   for ( PeakAssignments::const_iterator it = assignments_.begin(); it != assignments_.end(); ++it ) {
//     ++assignments_written;
//     if ( assignments_written > 1 ) os << std::endl << "                                            ";
//     for ( Size i=1; i<=2; i++ ) {
//       os << ObjexxFCL::fmt::RJ( 6, (*it)->resonance_id( i ) ) << " ";
//       if ( has_label( i ) ) {
// 	os << ObjexxFCL::fmt::RJ( 6, (*it)->label_resonance_id( i ) << " ";
//       }
//     }
//   }
// }



} //noesy_assign
} //devel
