// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/PoissonBoltzmannPotential.fwd.hh
/// @brief  PoissonBoltzmann potential class forward delcaration
/// @author Yifan Song (yfsong@uw.edu)

#ifndef INCLUDED_core_scoring_PoissonBoltzmannPotential_FWD_HH
#define INCLUDED_core_scoring_PoissonBoltzmannPotential_FWD_HH

#include <utility/pointer/owning_ptr.hh>
#include <utility/pointer/access_ptr.hh>

namespace core {
namespace scoring {

class PoissonBoltzmannPotential;

typedef utility::pointer::owning_ptr< PoissonBoltzmannPotential > PoissonBoltzmannPotentialOP;
typedef utility::pointer::owning_ptr< PoissonBoltzmannPotential const > PoissonBoltzmannPotentialCOP;
typedef utility::pointer::access_ptr< PoissonBoltzmannPotential > PoissonBoltzmannPotentialAP;
typedef utility::pointer::access_ptr< PoissonBoltzmannPotential const > PoissonBoltzmannPotentialCAP;
	//template <typename Key, typename Val> class HashTable;
	//typedef HashTable< std::string, bool > HashTable_str_bool;
}
}

#endif
