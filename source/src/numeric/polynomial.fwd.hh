// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.


/// @file numeric/polynomial.fwd.hh
/// @brief forward header for Polynomial class
/// @author Matthew O'Meara (mattjomeara@gmail.com)

#ifndef INCLUDED_numeric_polynomial_fwd_hh
#define INCLUDED_numeric_polynomial_fwd_hh

#include <utility/pointer/owning_ptr.hh>

namespace numeric {

class Polynomial_1d;

typedef utility::pointer::shared_ptr< Polynomial_1d > Polynomial_1dOP;
typedef utility::pointer::shared_ptr< Polynomial_1d const > Polynomial_1dCOP;

} //numeric

#endif // INCLUDED_numeric_polynomial_FWD_HH
