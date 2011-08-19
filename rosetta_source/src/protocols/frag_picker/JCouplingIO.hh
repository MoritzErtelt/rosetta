// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file   protocols/frag_picker/CSTalosIO.hh
/// @brief Class that reads and writes chemical shifts in TALOS format
/// @author Nikolas Sgourakis sgourn@u.w.edu

#ifndef INCLUDED_protocols_frag_picker_JCouplingIO_hh
#define INCLUDED_protocols_frag_picker_JCouplingIO_hh

// utility headers
#include <core/types.hh>

#include <string>
#include <map>

// boost headers
#include <boost/tuple/tuple.hpp>
// AUTO-REMOVED #include <utility/vector1.hh>

//Auto Headers
#include <utility/vector1_bool.hh>


namespace protocols {
namespace frag_picker {

using namespace core;

class JCouplingIO {
public:

	JCouplingIO() {}

	JCouplingIO(std::string file_name) {
		read(file_name);
	}

	void read(std::string const&);

	std::pair< Real, Real > get_data( Size const res_num, bool & has_data );

	utility::vector1< Real > get_parameters();

	Size get_length() {
		return sequence_length_;
	}

private:
	utility::vector1< utility::vector1< Real > > data_;
	Real A_, B_, C_, THETA_;
	Size sequence_length_;

};

} // frag_picker
} // protocols

#endif /* INCLUDED_protocols_frag_picker_JCouplingIO_HH */

