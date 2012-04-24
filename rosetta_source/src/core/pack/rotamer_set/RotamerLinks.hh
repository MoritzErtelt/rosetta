// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/pack/rotamer_set/RotamerSet.fwd.hh
/// @brief  Residue set class forward declarations
/// @author Andrew Leaver-Fay (leaverfa@email.unc.edu)


#ifndef INCLUDED_core_pack_rotamer_set_RotamerLinks_hh
#define INCLUDED_core_pack_rotamer_set_RotamerLinks_hh

#include <core/pack/rotamer_set/RotamerLinks.fwd.hh>

// Project Headers
#include <core/types.hh>

// utility headers
#include <utility/pointer/ReferenceCount.hh>
#include <utility/vector1.hh>

namespace core {
namespace pack {
namespace rotamer_set {

class RotamerLinks : public utility::pointer::ReferenceCount {

public:

	void
	resize( Size const res)
	{
		links_.resize(res);
	}

	utility::vector1< Size >
	get_equiv( Size const index ) const
	{
		if (index <= links_.size())
		return links_[index];
		return 0;	
	}

	void
	set_equiv( Size const rs1, Size const rs2 )
	{
		links_[rs1].push_back(rs2);
	}

	void
	set_equiv( Size const rs1, utility::vector1<int>  list)
	{
		links_[rs1] = list;
	}

	bool
	has (Size index) const
	{
		//return true;
		if (!links_[index].empty()){
			return true;
		}else{
			return false;
		}
	}


private:

	utility::vector1< utility::vector1< int > > links_;
};


} // namespace rotamer_set
} // namespace pack
} // namespace core


#endif //
