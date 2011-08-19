// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/features/StructureFeatures.hh
/// @brief  report Orbital geometry and scores to features Statistics Scientific Benchmark
/// @author Matthew O'Meara

#ifndef INCLUDED_protocols_features_StructureFeatures_hh
#define INCLUDED_protocols_features_StructureFeatures_hh

// Unit Headers
#include <protocols/features/FeaturesReporter.hh>
#include <protocols/features/StructureFeatures.fwd.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <utility/sql_database/DatabaseSessionManager.hh>
#include <utility/vector1.fwd.hh>

// C++ Headers
#include <string>

namespace protocols{
namespace features{

class StructureFeatures : public protocols::features::FeaturesReporter {
public:
	StructureFeatures();

	StructureFeatures( StructureFeatures const & src );

	virtual ~StructureFeatures();

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
		core::Size protocol_id,
		utility::sql_database::sessionOP db_session
	);

	///@brief collect all the feature data for the pose
	core::Size
	report_features(
		core::pose::Pose const & pose,
		utility::vector1< bool > const & relevant_residues,
		core::Size struct_id,
		core::Size protocol_id,
		utility::sql_database::sessionOP db_session
	);


	///@brief collect all the feature data for the pose
	core::Size
	report_features(
		core::pose::Pose const & pose,
		utility::vector1< bool > const & relevant_residues,
		core::Size protocol_id,
		utility::sql_database::sessionOP db_session,
		std::string const & tag
	);

	///@brief collect all the feature data for the pose
	///pass in struct_id explicitly to mange mergability in parallel runs.
	core::Size
	report_features(
		core::pose::Pose const & pose,
		utility::vector1< bool > const & relevant_residues,
		core::Size struct_id,
		core::Size protocol_id,
		utility::sql_database::sessionOP db_session,
		std::string const & tag
	);

	void
	load_into_pose(
		utility::sql_database::sessionOP db_session,
		core::Size struct_id,
		core::pose::Pose & pose);

	void
	load_tag(
		utility::sql_database::sessionOP db_session,
		core::Size struct_id,
		core::pose::Pose & pose);


	core::Size
	get_struct_id(
		utility::sql_database::sessionOP db_session,
		std::string const & tag);

};

} // namespace
} // namespace

#endif // include guard
