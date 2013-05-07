// -*- mode:c++;tab-width:2;indent-tabs-mode:nil;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/rotamer_recovery/RotamerRecovery.cxxtest.hh
/// @brief  Test FeaturesReporter class
/// @author Matthew O'Meara (mattjomeara@gmail.com)

// Test Headers
#include <test/core/init_util.hh>
#include <cxxtest/TestSuite.h>
#include <util/pose_funcs.hh>

// Unit Headers
#include <protocols/features/AtomAtomPairFeatures.hh>
#include <protocols/features/AtomInResidueAtomInResiduePairFeatures.hh>
#include <protocols/features/AtomTypesFeatures.hh>
#include <protocols/features/BetaTurnDetectionFeatures.hh>
#include <protocols/features/ChargeChargeFeatures.hh>
#include <protocols/features/GeometricSolvationFeatures.hh>
#include <protocols/features/HBondFeatures.hh>
#include <protocols/features/HBondParameterFeatures.hh>
#include <protocols/features/JobDataFeatures.hh>
#include <protocols/features/LoopAnchorFeatures.hh>
#include <protocols/features/OrbitalsFeatures.hh>
#include <protocols/features/PairFeatures.hh>
#include <protocols/features/PdbDataFeatures.hh>
#include <protocols/features/PoseCommentsFeatures.hh>
#include <protocols/features/PoseConformationFeatures.hh>
#include <protocols/features/ProteinBackboneTorsionAngleFeatures.hh>
#include <protocols/features/ProteinBackboneAtomAtomPairFeatures.hh>
#include <protocols/features/ProteinResidueConformationFeatures.hh>
#include <protocols/features/ProteinRMSDFeatures.hh>
#include <protocols/features/ProtocolFeatures.hh>
#include <protocols/features/RadiusOfGyrationFeatures.hh>
#include <protocols/features/ResidueFeatures.hh>
#include <protocols/features/ResidueScoresFeatures.hh>
#include <protocols/features/ResidueTypesFeatures.hh>
#include <protocols/features/ResidueBurialFeatures.hh>
#include <protocols/features/ResidueSecondaryStructureFeatures.hh>
#include <protocols/features/RotamerBoltzmannWeightFeatures.hh>
#include <protocols/features/RotamerRecoveryFeatures.hh>
#include <protocols/features/SaltBridgeFeatures.hh>
#include <protocols/features/StructureFeatures.hh>
#include <protocols/features/StructureScoresFeatures.hh>
#include <protocols/features/strand_assembly/SandwichFeatures.hh>
#include <protocols/features/strand_assembly/StrandBundleFeatures.hh>
#include <protocols/features/UnrecognizedAtomFeatures.hh>

// Project Headers
#include <basic/Tracer.hh>
#include <basic/database/sql_utils.hh>
#include <utility/sql_database/DatabaseSessionManager.hh>
#include <core/types.hh>
#include <core/pose/Pose.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

// Utility Headers
#include <utility/sql_database/DatabaseSessionManager.hh>
#include <utility/sql_database/types.hh>
#include <utility/file/file_sys_util.hh>

// Boost Headers
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <limits>

// External Headers
#include <cppdb/frontend.h>

//Auto Headers
#include <utility/vector1.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>
#include <basic/options/option.hh>


static basic::Tracer tr("protocols.features.FeaturesReporterTests.cxxtest");

class FeaturesReporterTests : public CxxTest::TestSuite {

public:

	void
	setUp() {

		using core::scoring::getScoreFunction;
		using utility::sql_database::DatabaseSessionManager;
		using namespace protocols::features;

		core_init();

		database_filename_ = "features_reporter_tests.db3";
		utility::file::file_delete(database_filename_);
    db_session_ = basic::database::get_db_session(database_filename_);

		//Need this to run the features reporter. Adds orbitals to residues
		//basic::options::option[ basic::options::OptionKeys::in::add_orbitals](true); // apl disabling as this screws up the singleton FA_STANDARD residue type set
		pose_1ten_ = fullatom_poseop_from_string( pdb_string_1ten() );

		relevant_residues_ = utility::vector1< bool >(pose_1ten_->total_residue(), true);
		batch_id_ = 0;

		score_function_ = getScoreFunction();

		score_function_->score(*pose_1ten_);

		structure_reporter_  = new StructureFeatures();

		features_reporters_.push_back(new AtomAtomPairFeatures());
		features_reporters_.push_back(new AtomInResidueAtomInResiduePairFeatures());
		features_reporters_.push_back(new AtomTypesFeatures());
		features_reporters_.push_back(new BetaTurnDetectionFeatures());
		features_reporters_.push_back(new ChargeChargeFeatures());
		features_reporters_.push_back(new GeometricSolvationFeatures());
		features_reporters_.push_back(new HBondFeatures(score_function_));
		features_reporters_.push_back(new HBondParameterFeatures(score_function_));
		features_reporters_.push_back(new JobDataFeatures());
		features_reporters_.push_back(new LoopAnchorFeatures());
		//features_reporters_.push_back(new OrbitalsFeatures());
		features_reporters_.push_back(new PairFeatures());
		features_reporters_.push_back(new PdbDataFeatures());
		features_reporters_.push_back(new PoseCommentsFeatures());
		features_reporters_.push_back(new PoseConformationFeatures());
		features_reporters_.push_back(new ProteinBackboneTorsionAngleFeatures());
		features_reporters_.push_back(new ProteinBackboneAtomAtomPairFeatures());
		features_reporters_.push_back(new ProteinResidueConformationFeatures());
		features_reporters_.push_back(new ProteinRMSDFeatures(pose_1ten_));
		features_reporters_.push_back(new RadiusOfGyrationFeatures());
		features_reporters_.push_back(new ResidueFeatures());
		features_reporters_.push_back(new ResidueScoresFeatures(score_function_));
		features_reporters_.push_back(new ResidueTypesFeatures());
		features_reporters_.push_back(new ResidueBurialFeatures());
		features_reporters_.push_back(new ResidueSecondaryStructureFeatures());
		features_reporters_.push_back(new RotamerBoltzmannWeightFeatures(score_function_));
		features_reporters_.push_back(new RotamerRecoveryFeatures(score_function_));
		features_reporters_.push_back(new SaltBridgeFeatures());
		features_reporters_.push_back(new StructureScoresFeatures(score_function_));
    features_reporters_.push_back(new strand_assembly::SandwichFeatures());
    features_reporters_.push_back(new strand_assembly::StrandBundleFeatures());
		features_reporters_.push_back(new UnrecognizedAtomFeatures());
	}

	void test_main() {
		write_full_schema(db_session_);
    do_test_type_name();
	}

	void write_full_schema(utility::sql_database::sessionOP db_session) {
		using protocols::features::FeaturesReporterOP;

		structure_reporter_->write_schema_to_db(db_session);

		foreach( FeaturesReporterOP const & reporter, features_reporters_ ){
			tr << "Writing schema for '" << reporter->type_name() << "'" << std::endl;
			reporter->write_schema_to_db(db_session);
		}
	}

  void do_test_type_name() {
    using protocols::features::FeaturesReporterOP;
    foreach(FeaturesReporterOP const & reporter, features_reporters_){
      TS_ASSERT_DIFFERS(reporter->type_name(), "Unknown_FeaturesReporter");
    }
  }

	void do_test_report_features() {
		using protocols::features::FeaturesReporterOP;
		using protocols::features::StructureID;

		tr << "Creating structure entry." << std::endl;
		StructureID parent_id = structure_reporter_->report_features(relevant_residues_, batch_id_, db_session_);
		tr << "Created structure id:" << parent_id << std::endl;

		foreach( FeaturesReporterOP const & reporter, features_reporters_ ){
			tr << "Reporting features for '" << reporter->type_name() << "'" << std::endl;
			reporter->report_features(*pose_1ten_, batch_id_, db_session_);
		}
	}

	void test_identifier_generation() {
		structure_reporter_->write_schema_to_db(db_session_);
		using protocols::features::FeaturesReporterOP;
		using protocols::features::StructureID;

		tr << "Creating structure entry." << std::endl;

		StructureID struct_id_one = structure_reporter_->report_features(relevant_residues_, batch_id_, db_session_);
		tr << "Created structure id:" << struct_id_one << std::endl;

		StructureID struct_id_two = structure_reporter_->report_features(relevant_residues_, batch_id_, db_session_);
		tr << "Created structure id:" << struct_id_two << std::endl;

		TS_ASSERT(struct_id_one == 1);
		TS_ASSERT(struct_id_two == 2);
		
		// Create a second database session in a separate partition
		// Delete the partitioned sqlite database
		utility::file::file_delete(database_filename_ + "_1");
		utility::sql_database::sessionOP db_session_p1 = utility::sql_database::DatabaseSessionManager::get_instance()->get_session_sqlite3(
			database_filename_,
			utility::sql_database::TransactionMode::standard,
			0,
			false,
			1);

		structure_reporter_->write_schema_to_db(db_session_p1);

		StructureID structure_prefix = 1;
		structure_prefix = structure_prefix << 32;

		tr << "Creating partitioned structure entry in database partition 1, structure prefix: " << structure_prefix << std::endl;

		StructureID struct_id_one_partition1 = structure_reporter_->report_features(relevant_residues_, batch_id_, db_session_p1);
		tr << "Created structure id:" << struct_id_one_partition1 << std::endl;

		StructureID struct_id_two_partition1 = structure_reporter_->report_features(relevant_residues_, batch_id_, db_session_p1);
		tr << "Created structure id:" << struct_id_two_partition1 << std::endl;


		TS_ASSERT(struct_id_one_partition1 == 1 + structure_prefix);
		TS_ASSERT(struct_id_two_partition1 == 2 + structure_prefix);
	}

private:
	core::pose::PoseOP pose_1ten_;
	core::scoring::ScoreFunctionOP score_function_;

	utility::vector1<bool> relevant_residues_;
	core::Size batch_id_;

	protocols::features::StructureFeaturesOP structure_reporter_;
	utility::vector1< protocols::features::FeaturesReporterOP > features_reporters_;

	std::string database_filename_;
	utility::sql_database::sessionOP db_session_;
};
