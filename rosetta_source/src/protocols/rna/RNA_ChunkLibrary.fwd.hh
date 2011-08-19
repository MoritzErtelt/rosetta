// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
//  vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/rna/RNA_ChunkLibrary.hh
/// @brief  Forward declarations header
/// @author rhiju


#ifndef INCLUDED_protocols_rna_RNA_ChunkLibrary_fwd_hh
#define INCLUDED_protocols_rna_RNA_ChunkLibrary_fwd_hh

// ObjexxFCL Headers
// AUTO-REMOVED #include <ObjexxFCL/ObjexxFCL.fwd.hh>
#include <utility/pointer/owning_ptr.hh>

// C++ Headers
// AUTO-REMOVED #include <string>
// AUTO-REMOVED #include <map>
// AUTO-REMOVED #include <vector>

namespace protocols{
namespace rna{

	class ChunkSet;
	class RNA_ChunkLibrary;

	typedef utility::pointer::owning_ptr< RNA_ChunkLibrary > RNA_ChunkLibraryOP;
	typedef utility::pointer::owning_ptr< ChunkSet > ChunkSetOP;

}
}

#endif
