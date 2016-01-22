// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet;
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/sewing/conformation/ContinuousAssembly.hh
///
/// @brief A set of simple container classes used by the SewingProtocol. These classes are used during the Hashing of coordinates, as well
/// as during assembly of low-resolution assemblies.
///
/// @author Tim Jacobs

#ifndef INCLUDED_protocols_sewing_conformation_ContinuousAssembly_hh
#define INCLUDED_protocols_sewing_conformation_ContinuousAssembly_hh

//Package headers


//Unit headers
#include <protocols/sewing/conformation/Assembly.hh>
#include <protocols/sewing/sampling/AppendAssemblyMover.fwd.hh>
#include <protocols/sewing/sampling/SewGraph.fwd.hh>

//Core headers

//Utility headers
#include <utility/pointer/owning_ptr.functions.hh>

//C++ headers

namespace protocols {
namespace sewing  {

/// @brief An Assembly is a collection of SewSegments. Assemblies are created using the
///geometric "compatibility" data generated by the Hasher.
class ContinuousAssembly : public protocols::sewing::Assembly {

public:

	/// @brief default constructor
	ContinuousAssembly();

	virtual
	AssemblyOP
	clone();

	/// @brief Needs to be implemented.
	virtual void
	append_model(
		Model const & model,
		ScoreResult const & edge_score
	);

};




} //sewing namespace
} //protocols namespace

#endif
