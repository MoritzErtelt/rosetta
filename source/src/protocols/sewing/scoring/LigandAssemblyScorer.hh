// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/sewing/scoring/LigandAssemblyScorer.hh
/// @brief an interface for making Requirements that deal with Assemblies
/// @author frankdt (frankdt@email.unc.edu)
/// @note   This is interface: it has no fields, and only
///         pure virtual methods.  No further constructors should
///         be defined.


#ifndef INCLUDED_protocols_sewing_scoring_LigandAssemblyScorer_hh
#define INCLUDED_protocols_sewing_scoring_LigandAssemblyScorer_hh


// Project forward headers
#include <protocols/sewing/scoring/LigandAssemblyScorer.fwd.hh>
#include <protocols/sewing/scoring/AssemblyScorer.hh>
#include <core/types.hh>
#include <protocols/sewing/data_storage/SmartAssembly.fwd.hh>


// Core headers

// Basic/Utility headers
#include <basic/datacache/DataMap.fwd.hh>
#include <utility/tag/Tag.fwd.hh>
namespace protocols {
namespace sewing {
namespace scoring {


/// @brief an interface for making Requirements that deal with Assemblies
class LigandAssemblyScorer : public AssemblyScorer {


public: // Types


protected: // Types




public: // Constants


protected: // Constants




public: // Creation


	/// @brief Destructor
	~LigandAssemblyScorer() override =default;


protected: // Creation


	/// @brief Prevent direct instantiation: No other constructors allowed.
	LigandAssemblyScorer()=default;
	LigandAssemblyScorer( LigandAssemblyScorer const & )=default;

public: // Methods
	// Further subsections of methods allowed

	core::Real
	score(data_storage::SmartAssemblyCOP assembly) override =0;

	std::string
	get_name() const override =0;

	void
	set_options_from_tag(
		utility::tag::TagCOP scorer_tag,
		basic::datacache::DataMap& datamap
	) override =0;
	/*
	static void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & );
	*/
	core::Real
	get_weight() const override =0;

	void
	set_weight( core::Real ) override =0;

	core::Real
	get_last_score() const override =0;

	void
	set_last_score( core::Real ) override =0;

	core::Real
	get_old_last_score() const override =0;

	void
	set_old_last_score( core::Real) override = 0;
protected: // Methods
	// Further subsections of methods allowed




public: // Properties


protected: // Properties

	// NO FIELDS ALLOWED


}; // LigandAssemblyScorer


} //protocols
} //sewing
} //scoring


#endif //INCLUDED_protocols_sewing_scoring_LigandAssemblyScorer_hh



