// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   test/core/scoring/methods/ProClosureEnergy.cxxtest.hh
/// @brief  test suite for core::scoring::ProClosureEnergy.cc
/// @author Andrew Leaver-Fay

// Test headers
#include <cxxtest/TestSuite.h>

// Unit headers
#include <core/scoring/methods/CartesianBondedEnergy.hh>

#include <platform/types.hh>

// Package Headers
#include <core/scoring/methods/EnergyMethodOptions.hh>
#include <test/util/pose_funcs.hh>
#include <test/util/deriv_funcs.hh>
#include <test/core/init_util.hh>
#include <core/kinematics/DomainMap.hh>

//Auto Headers
#include <utility/vector1.hh>


// --------------- Test Class --------------- //

// using declarations
using namespace core;
using namespace core::id;
using namespace core::kinematics;
using namespace core::pose;
using namespace core::scoring;
using namespace core::scoring::methods;

class CartesianBondedEnergyBBDepTests : public CxxTest::TestSuite {

	public:

	// Shared initialization goes here.
	void setUp() {
		using namespace core;
		core_init_with_additional_options( "-corrections:score:bbdep_bond_params");
	}

	// Shared finalization goes here.
	void tearDown() {}

	// --------------- Test Cases --------------- //
	void test_eval_energy()
	{
		Pose trpcage( create_trpcage_ideal_pose() );

		EnergyMethodOptions opts;
		CartesianBondedEnergy cartbond_energy( opts );
		ScoreFunction sfxn;
		sfxn.set_weight( cart_bonded, 0.5 );


		/*Size before_precision = std::cout.precision();
		std::cout.precision( 16 );
		for ( Size ii = 2; ii <= trpcage.total_residue(); ++ii ) {

			EnergyMap emap;
			std::cout << "{\n"
			"EnergyMap emap;\n";

			cartbond_energy.residue_pair_energy( trpcage.residue( ii-1 ), trpcage.residue( ii ), trpcage, sfxn, emap );
			std::cout << "cartbond_energy.residue_pair_energy( trpcage.residue( " << ii-1 << " ), trpcage.residue( " << ii << " ), trpcage, sfxn, emap );\n";
			std::cout << "TS_ASSERT_DELTA( emap[ cart_bonded ], " << emap[ cart_bonded ] << ", 1e-12 );\n";
			std::cout << "}\n";
		}
		std::cout.precision( before_precision );*/

		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 1 ), trpcage.residue( 2 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 6.968293646104515, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 2 ), trpcage.residue( 3 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.8909275044636028, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 3 ), trpcage.residue( 4 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 3.111314555249422, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 4 ), trpcage.residue( 5 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.3014325170163838, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 5 ), trpcage.residue( 6 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 1.77975110067326, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 6 ), trpcage.residue( 7 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.9650596084597788, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 7 ), trpcage.residue( 8 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.8859399081172433, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 8 ), trpcage.residue( 9 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.1863744488877972, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 9 ), trpcage.residue( 10 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 1.125469148785068, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 10 ), trpcage.residue( 11 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.03930233029517332, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 11 ), trpcage.residue( 12 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.03835009728494011, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 12 ), trpcage.residue( 13 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.5728329847982782, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 13 ), trpcage.residue( 14 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.9963137225847645, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 14 ), trpcage.residue( 15 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 15.64759174392195, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 15 ), trpcage.residue( 16 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.05331704731068346, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 16 ), trpcage.residue( 17 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 2.460850015440859, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 17 ), trpcage.residue( 18 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.5684130621501057, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 18 ), trpcage.residue( 19 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 0.565957465191082, 1e-12 );
		}
		{
			EnergyMap emap;
			cartbond_energy.residue_pair_energy( trpcage.residue( 19 ), trpcage.residue( 20 ), trpcage, sfxn, emap );
			TS_ASSERT_DELTA( emap[ cart_bonded ], 3.476952369677647, 1e-12 );
		}
	}

	void test_cartbonded_start_score_start_func_match_w_total_flexibility()
	{
		core::pose::Pose pose = create_trpcage_ideal_pose();
		core::scoring::ScoreFunction sfxn;
		sfxn.set_weight( cart_bonded, 0.5 );
		kinematics::MoveMap movemap( create_movemap_to_allow_all_torsions() );
		AtomDerivValidator adv;
		adv.set_pose( pose );
		adv.set_score_function( sfxn );
		adv.set_movemap( movemap );
		adv.validate_start_func_matches_start_score( 20.31722163820628, true );

	}

	void test_cartbonded_deriv_check_w_total_flexibility()
	{
		core::pose::Pose pose = create_trpcage_ideal_pose();
		core::scoring::ScoreFunction sfxn;
		sfxn.set_weight( cart_bonded, 0.5 );
		kinematics::MoveMap movemap( create_movemap_to_allow_all_torsions() );
		AtomDerivValidator adv;
		adv.set_pose( pose );
		adv.set_score_function( sfxn );
		adv.set_movemap( movemap );
		adv.simple_deriv_check( false, 1e-6 );

	}



};


