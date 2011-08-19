// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file   devel/DenovoProteinDesign/FragmentSequenceMover.fwd.hh
/// @brief  FragmentSequenceMover forward declarations header
/// @author

#ifndef INCLUDED_devel_denovo_protein_design_FragmentSequenceMover_fwd_hh
#define INCLUDED_devel_denovo_protein_design_FragmentSequenceMover_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace devel{
namespace denovo_protein_design{

//Forwards and OP typedefs
class FragmentSequenceMover;
typedef utility::pointer::owning_ptr< FragmentSequenceMover > FragmentSequenceMoverOP;
typedef utility::pointer::owning_ptr< FragmentSequenceMover const > FragmentSequenceMoverCOP;

}//DenovoProteinDesign
}//devel

#endif //INCLUDED_devel_DenovoProteinDesign_FragmentSequenceMover_FWD_HH
