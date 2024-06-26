// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/simple_moves/bin_transitions/PerturbByBinsCreator.hh
/// @brief This class will create instances of protocols::moves::Mover PerturbByBins for the protocols::moves::MoverFactory.
/// @author Vikram K. Mulligan (vmullig@uw.edu)

#ifndef INCLUDED_protocols_simple_moves_bin_transitions_PerturbByBinsCreator_hh
#define INCLUDED_protocols_simple_moves_bin_transitions_PerturbByBinsCreator_hh

#include <protocols/moves/MoverCreator.hh>

namespace protocols {
namespace simple_moves {
namespace bin_transitions {

class PerturbByBinsCreator : public protocols::moves::MoverCreator {
public:
	protocols::moves::MoverOP create_mover() const override;
	std::string keyname() const override;
	void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const override;
};

} //bin_transitions
} //simple_moves
} //protocols

#endif //INCLUDED_protocols_simple_moves_bin_transitions_PerturbByBinsCreator_hh

