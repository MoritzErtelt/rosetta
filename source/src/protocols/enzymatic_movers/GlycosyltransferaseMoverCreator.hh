// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/enzymatic_movers/GlycosyltransferaseMoverCreator.hh
/// @brief  Method declarations for RingConformationMoverCreator.
/// @author Labonte <JWLabonte@jhu.edu>


#ifndef INCLUDED_protocols_enzymatic_movers_GlycosyltransferaseMoverCreator_HH
#define INCLUDED_protocols_enzymatic_movers_GlycosyltransferaseMoverCreator_HH

// Project header
#include <protocols/moves/MoverCreator.hh>


namespace protocols {
namespace enzymatic_movers {

/// @brief  MoverCreator allowing the MoverFactory to create a GlycosyltransferaseMover
class GlycosyltransferaseMoverCreator : public moves::MoverCreator {

public:
	/// @brief  Return an up-casted owning pointer (MoverOP) to the mover.
	virtual moves::MoverOP create_mover() const;

	/// @brief  Return the string identifier for the associated Mover (GlycosyltransferaseMover).
	virtual std::string keyname() const;

	/// @brief  Static method that returns the keyname for performance reasons.
	static std::string mover_name();
};

}  // namespace enzymatic_movers
}  // namespace protocols

#endif  // INCLUDED_protocols_enzymatic_movers_GlycosyltransferaseMoverCreator_HH
