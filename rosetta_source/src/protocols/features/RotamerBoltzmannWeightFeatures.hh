// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/features/RotamerBoltzmannWeightFeatures.hh
/// @brief  report rotamer boltzmann weights to features Statistics Scientific Benchmark
/// @author Matthew O'Meara

#ifndef INCLUDED_protocols_features_RotamerBoltzmannWeightFeatures_hh
#define INCLUDED_protocols_features_RotamerBoltzmannWeightFeatures_hh

// Unit Headers
#include <protocols/features/FeaturesReporter.hh>
#include <protocols/features/RotamerBoltzmannWeightFeatures.fwd.hh>


// Project Headers
#include <core/types.hh>
#include <core/pose/Pose.hh>
#include <core/scoring/ScoreFunction.hh>
#include <protocols/protein_interface_design/filters/RotamerBoltzmannWeight.hh>
#include <utility/sql_database/DatabaseSessionManager.hh>
#include <utility/vector1.fwd.hh>

// C++ Headers
#include <string>

namespace protocols{
namespace features{

class RotamerBoltzmannWeightFeatures : public protocols::features::FeaturesReporter {
public:
	RotamerBoltzmannWeightFeatures();

	RotamerBoltzmannWeightFeatures(
		core::scoring::ScoreFunctionOP scfxn);


	RotamerBoltzmannWeightFeatures(RotamerBoltzmannWeightFeatures const & src);

	virtual ~RotamerBoltzmannWeightFeatures();

	///@brief return string with class name
	std::string
	type_name() const;

	///@brief return sql statements that setup the right tables
	std::string
	schema() const;

	///@brief collect all the feature data for the pose
	core::Size
	report_features(
		core::pose::Pose const & pose,
		utility::vector1< bool > const & relevant_residues,
		core::Size struct_id,
		utility::sql_database::sessionOP db_session);

private:
	protocols::protein_interface_design::filters::RotamerBoltzmannWeightFilterOP rotamer_boltzmann_weight_;

};

} // namespace
} // namespace

#endif // include guard
