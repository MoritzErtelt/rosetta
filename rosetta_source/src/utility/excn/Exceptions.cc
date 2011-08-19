// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file _Exception
/// @brief  base class for Exception system
/// @detailed responsibilities:
/// @author Oliver Lange

// Unit Headers
#include <utility/excn/Exceptions.hh>

// Package Headers
#include <string>
#include <ostream>

namespace utility {
namespace excn {

void EXCN_Msg_Exception::show( std::ostream& os ) const {
  os << msg() << std::endl;
}

}
}
