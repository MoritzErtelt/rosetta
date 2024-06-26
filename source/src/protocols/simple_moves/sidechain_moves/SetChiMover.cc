// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/simple_moves/sidechain_moves/SetChiMover.cc
/// @brief  A mover to change one chi angle
/// @author Noah Ollikanen

// Unit headers
#include <protocols/simple_moves/sidechain_moves/SetChiMover.hh>
#include <protocols/simple_moves/sidechain_moves/SetChiMoverCreator.hh>

// Project Headers
#include <core/types.hh>
#include <core/pose/Pose.hh>

#include <core/conformation/Residue.hh>
//parsing
#include <utility/tag/Tag.hh>
#include <core/pose/selection.hh>
#include <core/pose/ResidueIndexDescription.hh>
#include <basic/Tracer.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

namespace protocols {
namespace simple_moves {
namespace sidechain_moves {

using namespace core;
using namespace core::chemical;
using namespace std;

using core::pose::Pose;

static basic::Tracer TR( "protocols.simple_moves.sidechain_moves.SetChiMover" );




SetChiMover::~SetChiMover() = default;

/// @brief default ctor
SetChiMover::SetChiMover() :
	parent(),
	angle_( 0 ),
	resnum_( nullptr ),
	chinum_( 0 )
{}

core::Size
SetChiMover::resnum( core::pose::Pose const & pose ) const {
	if ( resnum_ == nullptr ) {
		return 0;
	} else {
		return resnum_->resolve_index( pose );
	}
}

void
SetChiMover::resnum( core::pose::ResidueIndexDescriptionCOP r ) {
	resnum_ = r;
}

void
SetChiMover::resnum( core::Size const r ) {
	resnum_ = core::pose::make_rid_posenum( r );
}

void SetChiMover::apply( Pose & pose ) {
	runtime_assert( resnum_ != nullptr );

	core::Size const resnum = resnum_->resolve_index( pose );

	if ( chinum() <= pose.residue(resnum).nchi() ) {
		pose.set_chi(chinum(), resnum, angle());
		TR<<"Set chi"<<chinum()<<" of residue "<<resnum<<" to "<<angle()<<std::endl;
	}

	pose.update_residue_neighbors();
}


void SetChiMover::parse_my_tag( utility::tag::TagCOP tag,
	basic::datacache::DataMap &
)
{
	angle( tag->getOption< core::Real >( "angle" ) );
	resnum( core::pose::parse_resnum( tag->getOption< std::string >( "resnum" ) ) );
	chinum( tag->getOption< core::Size >( "chinum" ) );

}

std::string SetChiMover::get_name() const {
	return mover_name();
}

std::string SetChiMover::mover_name() {
	return "SetChiMover";
}

void SetChiMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	// AMW no proper "defaults" but no requirement.
	using namespace utility::tag;
	AttributeList attlist;

	attlist + XMLSchemaAttribute("angle", xsct_real, "Angle to which to set the chi." )
		+ XMLSchemaAttribute("chinum", xsct_non_negative_integer, "Which chi is to be set." );

	core::pose::attributes_for_parse_resnum( attlist, "resnum", "Residue with the chi in question." );

	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "A mover to change one chi angle.", attlist );

}

std::string SetChiMoverCreator::keyname() const {
	return SetChiMover::mover_name();
}

protocols::moves::MoverOP
SetChiMoverCreator::create_mover() const {
	return utility::pointer::make_shared< SetChiMover >();
}

void SetChiMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	SetChiMover::provide_xml_schema( xsd );
}



} // sidechain_moves
} // simple_moves
} // protocols
