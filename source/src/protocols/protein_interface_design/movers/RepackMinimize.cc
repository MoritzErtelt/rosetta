// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/protein_interface_design/movers/RepackMinimize.cc
/// @brief
/// @author Sarel Fleishman (sarelf@u.washington.edu), Jacob Corn (jecorn@u.washington.edu)

// Unit headers
#include <protocols/protein_interface_design/movers/RepackMinimize.hh>
#include <protocols/protein_interface_design/movers/RepackMinimizeCreator.hh>

// Package headers
#include <protocols/protein_interface_design/design_utils.hh>
#include <protocols/symmetry/SetupForSymmetryMover.hh>

// Project headers
#include <core/pack/pack_rotamers.hh>
#include <core/select/residue_selector/ResidueIndexSelector.hh>
#include <core/pose/Pose.hh>
#include <basic/Tracer.hh>

#include <utility/vector1.hh>

//Auto Headers
#include <protocols/calc_taskop_movers/DesignRepackMover.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>


namespace protocols {
namespace protein_interface_design {
namespace movers {

using namespace core;
using namespace std;
using namespace core::scoring;
using namespace protocols::moves;

static basic::Tracer TR( "protocols.protein_interface_design.movers.RepackMinimize" );




protocols::moves::MoverOP
RepackMinimize::clone() const {
	return( utility::pointer::make_shared< RepackMinimize >( *this ) );
}

RepackMinimize::RepackMinimize() :
	calc_taskop_movers::DesignRepackMover( RepackMinimize::mover_name() )
{
	optimize_foldtree_ = true;
	automatic_repacking_definition_ = true;
}

RepackMinimize::RepackMinimize(
	ScoreFunctionCOP scorefxn_repack,
	ScoreFunctionCOP scorefxn_minimize,
	utility::vector1< core::Size > const & target_res,
	bool const repack_partner1/*=false*/,
	bool const repack_partner2/*=true*/,
	core::Real const interface_distance_cutoff/*=8.0*/,
	bool const repack_non_ala/*=true*/
) :
	calc_taskop_movers::DesignRepackMover( RepackMinimize::mover_name() )
{
	repack_partner2_ = repack_partner2;
	repack_partner1_ = repack_partner1;
	using namespace core::select::residue_selector;
	target_residues( utility::pointer::make_shared< ResidueIndexSelector >( target_res ) );
	interface_distance_cutoff_ = interface_distance_cutoff;
	scorefxn_repack_ = scorefxn_repack->clone();
	scorefxn_minimize_ = scorefxn_minimize->clone();
	repack_non_ala_ = repack_non_ala;
}

RepackMinimize::~RepackMinimize() = default;

/// @details designs interface residues and minimizes the pose.
/// If minimization parameters have not been set by the user the default minimization behaviour is as follows:
/// + minimize bb/sc for all residues that are repacked.
/// + minimize bb/sc of residues that are +-1 from target residues
/// + minimize bb for all residues (minimizing over only interface residues causes large motions during minimization)
/// + minimize rb jump
/// Note that some confusion may arise from the use of repack1_ and repack2_
/// The behaviour of these variables is such that if they're false, then the
/// relevant partner is not designed but may be repacked. Turning off design_
/// precludes design across the whole system only allowing repack. At one point
/// the names should be rethought...
void
RepackMinimize::apply( pose::Pose & pose )
{
	allowed_aas_[ chemical::aa_cys ] = false;
	allowed_aas_[ chemical::aa_gly ] = false;
	allowed_aas_[ chemical::aa_pro ] = false;

	if ( symmetry_ ) {
		protocols::symmetry::SetupForSymmetryMoverOP setup_mover( new protocols::symmetry::SetupForSymmetryMover );
		setup_mover->apply( pose );
	}

	setup_packer_and_movemap( pose );
	using namespace core::scoring;

	pack::pack_rotamers(pose, *scorefxn_repack_, task_ );
	if ( symmetry_ ) {
		SymMinimizeInterface( pose, scorefxn_minimize_, curr_min_bb_, curr_min_sc_, curr_min_rb_); //, optimize_foldtree_, target_residues(pose) );
	} else {
		MinimizeInterface( pose, scorefxn_minimize_, curr_min_bb_, curr_min_sc_, curr_min_rb_, optimize_foldtree_, target_residues(pose) );
	}
	pose.update_residue_neighbors();
	(*scorefxn_minimize_)( pose );
}



void
RepackMinimize::parse_my_tag( TagCOP const tag, basic::datacache::DataMap & data )
{
	TR<<"repack minimize mover with the following parameters:"<<std::endl;
	calc_taskop_movers::DesignRepackMover::parse_my_tag( tag, data );
}

std::string RepackMinimize::get_name() const {
	return mover_name();
}

std::string RepackMinimize::mover_name() {
	return "RepackMinimize";
}

void RepackMinimize::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	XMLSchemaComplexTypeGeneratorOP ct_gen = calc_taskop_movers::DesignRepackMover::get_xsd_complex_type();

	ct_gen->description( "RepackMinimizeMover performs extensive interface remodeling" );
	ct_gen->element_name( mover_name() );
	ct_gen->write_complex_type_to_schema( xsd );
}

std::string RepackMinimizeCreator::keyname() const {
	return RepackMinimize::mover_name();
}

protocols::moves::MoverOP
RepackMinimizeCreator::create_mover() const {
	return utility::pointer::make_shared< RepackMinimize >();
}

void RepackMinimizeCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	RepackMinimize::provide_xml_schema( xsd );
}


} //movers
} //protein_interface_design
} //protocols
