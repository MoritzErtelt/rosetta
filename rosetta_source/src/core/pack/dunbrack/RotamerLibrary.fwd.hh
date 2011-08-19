// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/pack/dunbrack/RotamerLibrary.fwd.hh
/// @brief  Score function class
/// @author Phil Bradley


#ifndef INCLUDED_core_pack_dunbrack_RotamerLibrary_fwd_hh
#define INCLUDED_core_pack_dunbrack_RotamerLibrary_fwd_hh

#include <core/conformation/Residue.fwd.hh>

// AUTO-REMOVED #include <utility/vector1.hh>
#include <utility/pointer/owning_ptr.hh>
#include <utility/pointer/access_ptr.hh>

namespace core {
namespace pack {
namespace dunbrack {

class ChiSet;
typedef utility::pointer::owning_ptr< ChiSet > ChiSetOP;


class RotamerLibrary;
typedef utility::pointer::owning_ptr< RotamerLibrary > RotamerLibraryOP;
typedef utility::pointer::owning_ptr< RotamerLibrary const > RotamerLibraryCOP;

class SingleResidueRotamerLibrary;
typedef utility::pointer::owning_ptr< SingleResidueRotamerLibrary > SingleResidueRotamerLibraryOP;
typedef utility::pointer::access_ptr< SingleResidueRotamerLibrary > SingleResidueRotamerLibraryAP;
typedef utility::pointer::owning_ptr< SingleResidueRotamerLibrary const > SingleResidueRotamerLibraryCOP;
typedef utility::pointer::access_ptr< SingleResidueRotamerLibrary > SingleResidueRotamerLibraryAP;
typedef utility::pointer::access_ptr< SingleResidueRotamerLibrary const > SingleResidueRotamerLibraryCAP;

class SingleResidueDunbrackLibrary;
typedef utility::pointer::owning_ptr< SingleResidueDunbrackLibrary > SingleResidueDunbrackLibraryOP;
typedef utility::pointer::access_ptr< SingleResidueDunbrackLibrary > SingleResidueDunbrackLibraryAP;
typedef utility::pointer::owning_ptr< SingleResidueDunbrackLibrary const > SingleResidueDunbrackLibraryCOP;
typedef utility::pointer::access_ptr< SingleResidueDunbrackLibrary const > SingleResidueDunbrackLibraryCAP;


typedef utility::vector1< conformation::ResidueOP > RotamerVector;


}
}
}

#endif
