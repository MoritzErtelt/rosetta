// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file       protocols/membrane/AddMembraneSpanTermZConstraintCreator.hh
/// @brief      Add membrane span constraint
/// @author     Jonathan Weinstein (jonathan.weinstein@weizmann.ac.il)

#ifndef INCLUDED_protocols_membrane_AddMembraneSpanTermZConstraint_cc
#define INCLUDED_protocols_membrane_AddMembraneSpanTermZConstraint_cc

// Unit Headers
#include <protocols/membrane/AddMembraneSpanTermZConstraint.hh>
#include <protocols/membrane/AddMembraneSpanTermZConstraintCreator.hh>
#include <protocols/moves/Mover.hh>

// Project Headers

// Package Headers
#include <core/pose/Pose.hh>

#include <core/scoring/constraints/Constraint.fwd.hh>
#include <core/scoring/constraints/MembraneSpanTermZConstraint.hh>

// C++ Headers
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

#include <basic/Tracer.hh> // AUTO IWYU For Tracer

static basic::Tracer TR( "protocols.membrane.AddMembraneSpanTermZConstraint" );

namespace protocols {
namespace membrane {

/////////////////////
/// Constructors  ///
/////////////////////

/// @brief Default Constructor
AddMembraneSpanTermZConstraint::AddMembraneSpanTermZConstraint() : protocols::moves::Mover()
{
}

/// @brief Copy Constructor
/// @details Create a deep copy of this mover
AddMembraneSpanTermZConstraint::AddMembraneSpanTermZConstraint( AddMembraneSpanTermZConstraint const & ) = default;

/// @brief Assignment Operator
AddMembraneSpanTermZConstraint & AddMembraneSpanTermZConstraint::operator = ( AddMembraneSpanTermZConstraint const & src ) {

	// Abort self-assignment.
	if ( this == &src ) {
		return *this;
	}

	// Otherwise, create a new object
	return *( new AddMembraneSpanTermZConstraint( *this ) ); // ?????????
}

/// @brief Destructor
AddMembraneSpanTermZConstraint::~AddMembraneSpanTermZConstraint() = default;

///////////////////////////////
/// Rosetta Scripts Methods ///
///////////////////////////////

/// @brief Create a Clone of this mover
protocols::moves::MoverOP
AddMembraneSpanTermZConstraint::clone() const {
	return ( utility::pointer::make_shared< AddMembraneSpanTermZConstraint >( *this ) );
}

/// @brief Create a Fresh Instance of this Mover
protocols::moves::MoverOP
AddMembraneSpanTermZConstraint::fresh_instance() const {
	return utility::pointer::make_shared< AddMembraneSpanTermZConstraint >();
}

/// @brief Pase Rosetta Scripts Options for this Mover
void
AddMembraneSpanTermZConstraint::parse_my_tag(
	utility::tag::TagCOP,
	basic::datacache::DataMap &
) {
}

/// @brief Create a new copy of this mover

/// @brief Return the Name of this mover (as seen by Rscripts)

/// @brief Mover name for Rosetta Scripts


/////////////////////
/// Mover Methods ///
/////////////////////

/// @brief Get the name of this Mover (AddMembraneSpanTermZConstraint)
std::string
AddMembraneSpanTermZConstraint::get_name() const {
	return "AddMembraneSpanTermZConstraint";
}

/// @brief Flip the downstream partner in the membrane
void AddMembraneSpanTermZConstraint::apply( core::pose::Pose & pose ) {
	using namespace core::scoring::constraints;

	ConstraintCOP cst( new core::scoring::constraints::MembraneSpanTermZConstraint( pose ) );
	pose.add_constraint( cst );

}// apply


std::string AddMembraneSpanTermZConstraint::mover_name() {
	return "AddMembraneSpanTermZConstraint";
}

void AddMembraneSpanTermZConstraint::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "Add membrane span delta Z of span termini constraint", attlist );
}

std::string AddMembraneSpanTermZConstraintCreator::keyname() const {
	return AddMembraneSpanTermZConstraint::mover_name();
}

protocols::moves::MoverOP
AddMembraneSpanTermZConstraintCreator::create_mover() const {
	return utility::pointer::make_shared< AddMembraneSpanTermZConstraint >();
}

void AddMembraneSpanTermZConstraintCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	AddMembraneSpanTermZConstraint::provide_xml_schema( xsd );
}



} // membrane
} // protocols

#endif // INCLUDED_protocols_membrane_AddMembraneSpanTermZConstraint_cc
