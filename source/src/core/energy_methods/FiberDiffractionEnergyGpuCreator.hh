// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/core/energy_methods/FiberDiffractionEnergyCreator.hh
/// @brief  Energy Creator for FiberDiffractionEnergy
/// @author Ingemar Andre

#ifndef INCLUDED_core_scoring_fiber_diffraction_FiberDiffractionEnergyGpuCreator_hh
#define INCLUDED_core_scoring_fiber_diffraction_FiberDiffractionEnergyGpuCreator_hh

#include <core/scoring/methods/EnergyMethodCreator.hh>
#include <core/types.hh>

namespace core {
namespace energy_methods {

#ifdef USECUDA
class FiberDiffractionEnergyGpuCreator : public core::scoring::methods::EnergyMethodCreator {
public:

	FiberDiffractionEnergyGpuCreator() {}

	/// @brief Instantiate a new FiberDiffractionEnergy
	virtual core::scoring::methods::EnergyMethodOP create_energy_method( core::scoring::methods::EnergyMethodOptions const &) const;

	/// @brief Return the set of score types claimed by the EnergyMethod
	/// this EnergyMethodCreator creates in its create_energy_method() function
	virtual ScoreTypes score_types_for_method() const;
};
#endif

}
}

#endif
