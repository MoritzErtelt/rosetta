// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/ScoreTest.cxxtest.hh
/// @brief  unified scoring test.
/// @author Sergey Lyskov

// Test headers
#include <cxxtest/TestSuite.h>

// Project Headers
#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>

#include <core/chemical/ResidueTypeSet.hh>
#include <core/chemical/ChemicalManager.hh>

#include <core/conformation/Residue.hh>
#include <core/conformation/ResidueFactory.hh>

#include <core/kinematics/FoldTree.hh>

#include <core/pose/Pose.hh>

#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <core/types.hh>

// Package headers
#include <core/io/pdb/pose_io.hh>

#include <basic/Tracer.hh>
#include <test/UTracer.hh>

//Auto Headers
#include <core/chemical/AtomType.hh>
#include <core/chemical/AtomTypeSet.hh>
#include <core/chemical/VariantType.hh>
#include <core/id/DOF_ID.hh>
#include <core/id/NamedStubID.hh>
#include <core/io/pdb/file_data.hh>
#include <core/import_pose/import_pose.hh>
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

static basic::Tracer TR("core.scoring.VirtualResidues.cxxtest");

// using declarations
using namespace core;
using namespace scoring;
using namespace conformation;
using namespace chemical;
using namespace scoring;
using namespace pose;

///////////////////////////////////////////////////////////////////////////
/// @name ScoreTest
/// @brief: unified tests for difference score functions/methods
///////////////////////////////////////////////////////////////////////////
class VirtualResidueScoreTest : public CxxTest::TestSuite {

public:

	void setUp() {
		core_init( );
	}

	void tearDown() {}

	void test_centroid_vrt_scores() {
		core::Real tol = 1e-6;

		pose::Pose pose;
		core::import_pose::centroid_pose_from_pdb( pose,"core/scoring/test_in.pdb");

		core::scoring::ScoreFunctionOP scorefxn
		          = core::scoring::ScoreFunctionFactory::create_score_function( "cen_std" );

		// center pose at origin
		numeric::xyzVector< core::Real > com(0.0,0.0,0.0);
		for (int i=1; i<=(int)pose.total_residue(); ++i)
			com +=pose.residue(i).atom(2).xyz();
		com /= pose.total_residue();

		for (int i=1; i<=(int)pose.total_residue(); ++i) {
			core::conformation::Residue const &rsd ( pose.residue(i) );
			for (int j=1; j<=(int)rsd.natoms(); j++) {
				core::id::AtomID id( j, i );
				pose.set_xyz( id, pose.xyz(id) - com );
			}
		}

		// 1) score structure
		core::Real score1 = (*scorefxn)(pose);

		// add jump to vrt res
		TS_ASSERT( pose.residue(1).residue_type_set().has_name("VRT") );

		if (pose.residue(1).residue_type_set().has_name("VRT")) {
			pose.append_residue_by_jump
				 ( *core::conformation::ResidueFactory::create_residue( pose.residue(1).residue_type_set().name_map( "VRT" ) ),
						pose.total_residue()/2 );

			// make the virt atom the root
			kinematics::FoldTree newF(pose.fold_tree());
			newF.reorder( pose.total_residue() );
			pose.fold_tree( newF );

			// 2) rescore structure w/ virtual res
			core::Real score2 = (*scorefxn)(pose);
			core::Real score_dev = std::fabs(score1 - score2);

			TR << "centroid-mode score deviation = " << score_dev << std::endl;
			TS_ASSERT( score_dev < tol );
		}
	}


	void test_fa_vrt_scores() {
		core::Real tol = 1e-6;

		pose::Pose pose( create_test_in_pdb_pose() );
		//core::import_pose::pose_from_pdb( pose,"core/scoring/test_in.pdb");

		core::scoring::ScoreFunctionOP scorefxn
		          = core::scoring::ScoreFunctionFactory::create_score_function( "score13_env_hb" );

		// center pose at origin
		numeric::xyzVector< core::Real > com(0.0,0.0,0.0);
		for (int i=1; i<=(int)pose.total_residue(); ++i)
			com +=pose.residue(i).atom(2).xyz();
		com /= pose.total_residue();

		for (int i=1; i<=(int)pose.total_residue(); ++i) {
			core::conformation::Residue const &rsd ( pose.residue(i) );
			for (int j=1; j<=(int)rsd.natoms(); j++) {
				core::id::AtomID id( j, i );
				pose.set_xyz( id, pose.xyz(id) - com );
			}
		}

		// 1) score structure
		core::Real score1 = (*scorefxn)(pose);
		//scorefxn->show( std::cerr , pose );

		// add jump to vrt res
		pose.append_residue_by_jump
		   ( *core::conformation::ResidueFactory::create_residue( pose.residue(1).residue_type_set().name_map( "VRT" ) ),
		      pose.total_residue()/2 );

		// make the virt atom the root
		kinematics::FoldTree newF(pose.fold_tree());
		newF.reorder( pose.total_residue() );
		pose.fold_tree( newF );

		// 2) rescore structure w/ virtual res
		core::Real score2 = (*scorefxn)(pose);
		//scorefxn->show( std::cerr , pose );
		core::Real score_dev = std::fabs(score1 - score2);

		TR << "full-atom score deviation = " << score_dev << std::endl;
		TS_ASSERT( score_dev < tol );
	}
};
