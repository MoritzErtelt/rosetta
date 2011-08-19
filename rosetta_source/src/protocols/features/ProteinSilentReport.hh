// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/features/ProteinSilentReport.hh
/// @brief  report feature data to database
/// @author Matthew O'Meara

#ifndef INCLUDED_protocols_features_ProteinSilentReport_hh
#define INCLUDED_protocols_features_ProteinSilentReport_hh

// Unit Headers
#include <protocols/features/Report.hh>
#include <protocols/features/ProteinSilentReport.fwd.hh>

// Project Headers
#include <protocols/features/ProtocolFeatures.hh>
#include <protocols/features/StructureFeatures.hh>
#include <protocols/features/StructureScoresFeatures.hh>
#include <protocols/features/PoseConformationFeatures.hh>
#include <protocols/features/PoseCommentsFeatures.hh>
#include <protocols/features/ProteinResidueConformationFeatures.hh>
#include <protocols/features/ResidueConformationFeatures.hh>

// Platform Headers
#include <core/types.hh>
#include <core/pose/Pose.fwd.hh>
#include <utility/sql_database/DatabaseSessionManager.hh>

// C++ Headers
#include <map>

namespace protocols{
namespace features{

class ProteinSilentReport : public protocols::features::Report {

public:
	typedef std::map< core::Size, core::pose::PoseOP > StructureMap;

public:
	ProteinSilentReport();

	ProteinSilentReport(ProteinSilentReport const & src);

	virtual ~ProteinSilentReport();

	core::Size
	version();

	void
	apply(
		core::pose::Pose const & pose,
		utility::sql_database::sessionOP db_sesion);

	void
	apply(
		core::pose::Pose const & pose,
		utility::sql_database::sessionOP db_sesion,
		std::string const & tag);

	void
	load_pose(
		utility::sql_database::sessionOP db_session,
		std::string tag,
		core::pose::Pose & pose);

	void
	write_schema_to_db(
		utility::sql_database::sessionOP db_session ) const;

private:

	bool initialized_;

	core::Size protocol_id_;
	StructureMap structure_map_;

	protocols::features::ProtocolFeaturesOP protocol_features_;
	protocols::features::StructureFeaturesOP structure_features_;
	protocols::features::StructureScoresFeaturesOP structure_scores_features_;
	protocols::features::PoseConformationFeaturesOP pose_conformation_features_;
	protocols::features::PoseCommentsFeaturesOP pose_comments_features_;
	protocols::features::ProteinResidueConformationFeaturesOP protein_residue_conformation_features_;
	protocols::features::ResidueConformationFeaturesOP residue_conformation_features_;

};

} //namespace
} //namespace

#endif //include guard
