// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/constraint_generator/RemoveConstraints.hh
/// @brief Removes constraints generated by a ConstraintGenerator
/// @author Tom Linsky (tlinsky@uw.edu)

#ifndef INCLUDED_protocols_constraint_generator_RemoveConstraints_hh
#define INCLUDED_protocols_constraint_generator_RemoveConstraints_hh

// Unit headers
#include <protocols/constraint_generator/RemoveConstraints.fwd.hh>
#include <protocols/moves/Mover.hh>

// Protocol headers
#include <protocols/constraint_generator/ConstraintGenerator.fwd.hh>
#include <protocols/filters/Filter.fwd.hh>

// Core headers
#include <core/pose/Pose.fwd.hh>

// Basic/Utility headers
#include <basic/datacache/DataMap.fwd.hh>
#include <utility/excn/Exceptions.hh>

namespace protocols {
namespace constraint_generator {

///@brief Removes constraints generated by a ConstraintGenerator
class RemoveConstraints : public protocols::moves::Mover {

public:
	RemoveConstraints();
	RemoveConstraints(
		ConstraintGeneratorCOPs const & generators,
		bool const exception_on_failure );

	// destructor (important for properly forward-declaring smart-pointer members)
	~RemoveConstraints() override;


	void
	apply( core::pose::Pose & pose ) override;

public:

	/// @brief parse XML tag (to use this Mover in Rosetta Scripts)
	void parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data,
		protocols::filters::Filters_map const & filters,
		protocols::moves::Movers_map const & movers,
		core::pose::Pose const & pose ) override;

	/// @brief required in the context of the parser/scripting scheme
	moves::MoverOP
	fresh_instance() const override;

	/// @brief required in the context of the parser/scripting scheme
	protocols::moves::MoverOP
	clone() const override;

public:
	void
	add_generator( protocols::constraint_generator::ConstraintGeneratorCOP generator );

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


private:
	ConstraintGeneratorCOPs generators_;
	bool exception_on_failure_;

};

class EXCN_RemoveCstsFailed : public utility::excn::Exception {
public:
	EXCN_RemoveCstsFailed(char const *file, int line, std::string const &m):
		utility::excn::Exception(file, line, "Remodel constraints somehow got lost along the way\n")
	{
		add_msg(m);
	}
};

} //protocols
} //constraint_generator

#endif //protocols/constraint_generator_RemoveConstraints_hh
