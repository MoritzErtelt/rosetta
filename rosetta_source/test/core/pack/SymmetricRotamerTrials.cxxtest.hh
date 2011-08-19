// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/pack/RotamerTrials.cxxtest.hh
/// @brief  test suite for rotamer_trials
/// @author Phil Bradley
/// @author Sergey Lyskov
/// @author P. Douglas Renfrew (renfrew@unc.edu)

// Test headers
#include <cxxtest/TestSuite.h>

#include "platform/types.hh"

#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>

#include <core/chemical/ResidueTypeSet.hh>
#include <core/chemical/ChemicalManager.hh>
#include <core/conformation/Residue.hh>

#include <core/io/pdb/pose_io.hh>

#include <core/pack/rotamer_trials.hh>
#include <core/pack/rtmin.hh>
#include <core/pack/task/PackerTask.hh>
#include <core/pack/task/TaskFactory.hh>

#include <core/pose/Pose.hh>

#include <core/scoring/symmetry/SymmetricScoreFunction.hh>

#include <core/pose/symmetry/util.hh>

#include <core/types.hh>

#include <basic/Tracer.hh>

#include <numeric/random/random.hh>
#include <numeric/angle.functions.hh>

#include <test/UTracer.hh>

//Auto Headers
#include <core/chemical/AtomType.hh>
#include <core/chemical/AtomTypeSet.hh>
#include <core/chemical/VariantType.hh>
#include <core/id/DOF_ID.hh>
#include <core/id/NamedStubID.hh>
#include <core/io/pdb/file_data.hh>
#include <core/import_pose/import_pose.hh>
#include <core/pose/datacache/CacheableDataType.hh>
#include <core/pose/signals/ConformationEvent.hh>
#include <core/pose/signals/DestructionEvent.hh>
#include <core/pose/signals/EnergyEvent.hh>
#include <core/scoring/types.hh>
#include <core/scoring/constraints/Constraints.fwd.hh>
#include <utility/stream_util.hh>
#include <utility/keys/Key2Tuple.hh>
#include <ObjexxFCL/FArray.fwd.hh>


using basic::T;
using basic::Error;
using basic::Warning;

static basic::Tracer TR("core.pack.RotamerTrials.cxxtest");

using namespace core;

class SymmetricRotamerTrials : public CxxTest::TestSuite
{
	chemical::ResidueTypeSetCAP residue_set;
	Real delta;

public:
	SymmetricRotamerTrials() {};

	// Shared initialization goes here.
	void setUp() {
		core_init_with_additional_options( "-no_optH -symmetry:symmetry_definition core/scoring/symmetry/sym_def.dat" );

		residue_set = chemical::ChemicalManager::get_instance()->residue_type_set( chemical::FA_STANDARD );

		// init delta
		delta = 0.01;
	}

	// Shared finalization goes here.
	void tearDown() {
	}


///////////////////////////////////////////////////////////////////////////////
// ------------------------------------------ //
/// @brief test for rotamer_trials
void test_symmetric_rotamer_trials()
{
	// UTracer log file
	test::UTracer UT("core/pack/SymmetricRotamerTrials.u");
	do_symmetric_rotamer_trials(false, UT);
}

/// @brief test for rotamer trials with minimization
/*void notest_rotamer_trials_with_minimization()
{
	// UTracer log file
	test::UTracer  UT("core/pack/SymmetricRTMIN.u");
	do_rotamer_trials(true, UT);
}
*/
void do_symmetric_rotamer_trials(bool /*with_minimization*/, test::UTracer & UT)
{
	using namespace conformation;
	using namespace chemical;
	using namespace scoring;
	using namespace scoring::symmetry;
	using namespace pose;

	// init/reset seeds in all RG objects we have to do this inside the test it self function since
	// user could request to run just one singel test.
	core::init_random_generators(1101, numeric::random::_RND_TestRun_, "mt19937");

	SymmetricScoreFunction scorefxn;
	scorefxn.set_weight( fa_atr, 0.80 );
	scorefxn.set_weight( fa_rep, 0.44 );
	scorefxn.set_weight( fa_sol, 0.65 );
	scorefxn.set_weight( fa_pair, 0.49 );

	scorefxn.set_weight( hbond_sr_bb, 1.0 );
	scorefxn.set_weight( hbond_lr_bb, 1.0 );
	scorefxn.set_weight( hbond_bb_sc, 1.0 );
	scorefxn.set_weight( hbond_sc, 1.0 );


	// read in pose
	//Pose pose(create_test_in_pdb_pose());
	//core::import_pose::pose_from_pdb( pose, "core/pack/test_in.pdb" );
	pose::Pose pose;
	core::import_pose::pose_from_pdb( pose, "core/scoring/symmetry/test_in.pdb" );
	core::pose::symmetry::make_symmetric_pose( pose );

	// calculate original score
	Energy score_orig = scorefxn( pose );

	// create paker task for rotamer trials
	pack::task::PackerTaskOP task
		( pack::task::TaskFactory::create_packer_task( pose ));
	task->initialize_from_command_line().restrict_to_repacking();

	// run rotamer trials given our pose, score function and packer task
	// symmetry does not work with RTMIN yet...
	/*
	if(with_minimization) {
		// Just run RTMIN on a few residues, because it's slow
		for(Size i = 1; i < 30; ++i) task->nonconst_residue_task(i).prevent_repacking();
		for(Size i = 115; i <= pose.total_residue(); ++i) task->nonconst_residue_task(i).prevent_repacking();
		pack::rtmin( pose, scorefxn, task );
	}
	else pack::symmetric_rotamer_trials( pose, scorefxn, task );
	*/

	pack::symmetric_rotamer_trials( pose, scorefxn, task );

	// calculate score after rotamer trials
	Energy score_rt = scorefxn( pose );

	pose.dump_pdb( "pose_out.pdb" );

	// test that scores are lower after running rotamer trials
	TS_ASSERT_LESS_THAN(score_rt, score_orig);

	// test that the score are the same as when the test was created
	// note: last update will sheffler (willsheffler@gmail.com) march 24 2008
	//Energy precomputed_score_orig = 44.2714; //  56.684;
	//Energy precomputed_score_rt = -152.9630;//-155.6311; //-144.5115;
	//TS_ASSERT_DELTA( precomputed_score_orig, score_orig, delta );
	//TS_ASSERT_DELTA( precomputed_score_rt, score_rt, delta );

	UT.abs_tolerance(delta);
	UT << "score_orig = " << score_orig << std::endl;
	UT << "score_rt = " << score_rt << std::endl;

	// test that the rotamers produced are the same as when the test was created
	// note: last update will sheffler (willsheffler@gmail.com) march 24 2008

	Size nres = pose.n_residue();
	for ( Size i = 1; i <= nres; ++i ) {
		// get chi angles
		utility::vector1< Real > res_chi = pose.residue( i ).chi();

		// compares values to those in the list
		for ( Size j = 1; j <= res_chi.size(); ++j ) {
			Real chi = numeric::nonnegative_principal_angle_degrees( res_chi[ j ] );
			//TR << std::setprecision(3) << std::fixed << std::setw(7) << chi << ", ";
			// std::cerr << "FOOCHI " << chi << std::endl;

			UT << std::setprecision(15);
			//TS_ASSERT_DELTA( precomputed_chi_angles[precompute_counter], chi, delta );
			UT << "residue = " << i << " chi = " << chi << std::endl;
		}
	}
};

};
