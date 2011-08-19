// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/saxs/SAXSEnergy.hh
/// @brief  "Energy" based on a similarity of theoretical SAXS spectrum computed for a pose and the experimental data
/// @author Dominik Gront (dgront@chem.uw.edu.pl)


#ifndef INCLUDED_core_scoring_saxs_SAXSEnergyFA_hh
#define INCLUDED_core_scoring_saxs_SAXSEnergyFA_hh

// Package headers
#include <core/scoring/saxs/FormFactorManager.hh>
#include <core/scoring/saxs/FormFactor.hh>
#include <core/scoring/saxs/SAXSEnergyCreatorFA.hh>
#include <core/scoring/saxs/SAXSEnergy.hh>

#include <core/scoring/EnergyMap.hh>
#include <core/scoring/methods/WholeStructureEnergy.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>

#include <core/scoring/ScoreType.hh>
#include <core/scoring/ScoreFunction.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

#include <string>

namespace core {
namespace scoring {
namespace saxs {


class SAXSEnergyFA : public SAXSEnergy  {
public:

    /// c-tor
    SAXSEnergyFA() : SAXSEnergy( fa_cfg_file_,
	    chemical::ChemicalManager::get_instance()->residue_type_set(core::chemical::FA_STANDARD),saxs_fa_score,new SAXSEnergyCreatorFA) {}

};


}
}
}

#endif
