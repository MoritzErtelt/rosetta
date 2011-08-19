// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet;
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available
// (c) under license. The Rosetta software is developed by the contributing
// (c) members of the Rosetta Commons. For more information, see
// (c) http://www.rosettacommons.org. Questions about this can be addressed to
// (c) University of Washington UW TechTransfer,
// (c) email: license@u.washington.edu.

/// @file   Ubiquitin_E2C_Modeler.fwd.hh
/// @brief forward declaration
/// @author Aroop Sircar (aroopsircar@yahoo.com)

#ifndef INCLUDED_protocols_antibody_Ubiquitin_E2C_Modeler_fwd_hh
#define INCLUDED_protocols_antibody_Ubiquitin_E2C_Modeler_fwd_hh

#include <utility/pointer/owning_ptr.hh>

namespace protocols {
	namespace ub_e2c {

		class ubi_e2c_modeler;
		typedef utility::pointer::owning_ptr< ubi_e2c_modeler > ubi_e2c_modelerOP;
		typedef utility::pointer::owning_ptr< ubi_e2c_modeler const >
		ubi_e2c_modeler_COP;

	} // namespace ub_e2c
} // namespace protocols

#endif

