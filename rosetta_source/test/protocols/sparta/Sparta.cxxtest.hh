// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

#include <cxxtest/TestSuite.h>
#include <test/core/init_util.hh>

#include <core/types.hh>
#include <core/pose/Pose.hh>
#include <core/import_pose/import_pose.hh>
#include <protocols/sparta/Sparta.hh>

#include <utility/vector1.hh>
#include <numeric>

static numeric::random::RandomGenerator rg(840184); // <- Magic number, do not change it!!!

class SpartaTest : public CxxTest::TestSuite {

private:

public:

	void setUp() {
		core_init();
	}

	void tearDown() {}

	core::Real calc_sparta_score(
		std::string const & pdb_file,
		std::string const & cs_file
	) {
		using core::Size;
		using core::Real;
		using std::string;
		using utility::vector1;
		using namespace protocols::sparta;
		core::pose::Pose pose;
		core::import_pose::pose_from_pdb(pose,pdb_file);
		Sparta sparta(cs_file);

		Real const sparta_score(sparta.score_pose(pose));
		std::cout << "score(" << pdb_file << "," << cs_file << ") = " << sparta_score <<
			std::endl;
		return sparta_score;
	}

	void test_sparta_foldit2() {
		using core::Real;
		using std::string;
		string const pdb_fn( "protocols/sparta/SAM-T08-server_TS3.pdb" );
		string const cs_fn ( "protocols/sparta/17280_trim.tab" );
		Real const sparta_score( calc_sparta_score(pdb_fn,cs_fn) );

		std::cout << sparta_score << std::endl;
	}

	void test_sparta_2kywA() {
		using core::Size;
		using core::Real;
		using std::string;
		using utility::vector1;
		using namespace protocols::sparta;
		string const cs_file ( "protocols/sparta/data_16988.tab" );
		string const pdb_file( "protocols/sparta/2kywA.pdb" );
		float const TOLERATED_ERROR( 5e-3 );

		Sparta sparta(cs_file);

		core::pose::Pose pose;
		core::import_pose::pose_from_pdb(pose,pdb_file);

		Real const sparta_score(sparta.score_pose(pose));
		vector1< float > scores( sparta.score_pose_per_residue(pose) );
		float sum_scores( std::accumulate( scores.begin(), scores.end(), 0.0 ) );

		TS_ASSERT_DELTA( sparta_score, 180.359, TOLERATED_ERROR );

		TS_ASSERT( scores.size() == 87 );
		TS_ASSERT_DELTA( sparta_score, sum_scores/4, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[1],   0.000, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[2],   8.155, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[3],   4.719, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[4],   5.612, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[5],   9.410, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[6],  13.496, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[7],  13.152, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[8],   8.785, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[9],   7.969, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[10],  7.243, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[11], 18.666, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[12], 10.722, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[13],  6.555, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[14],  5.923, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[15],  1.538, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[16],  0.255, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[17],  3.170, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[18],  1.618, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[19],  5.050, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[20],  3.058, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[21],  6.085, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[22],  3.454, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[23],  5.593, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[24],  9.331, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[25],  4.659, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[26],  3.274, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[27],  5.287, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[28], 15.773, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[29], 17.096, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[30], 16.593, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[31],  6.208, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[32], 15.087, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[33],  9.099, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[34],  4.873, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[35],  1.842, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[36],  3.676, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[37],  5.783, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[38],  8.417, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[39],  6.001, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[40],  3.980, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[41],  1.382, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[42],  1.904, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[43],  3.840, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[44],  4.532, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[45],  4.740, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[46],  1.146, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[47],  3.023, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[48],  3.215, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[49], 15.225, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[50],  5.657, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[51], 96.665, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[52], 21.381, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[53], 12.988, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[54],  2.452, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[55],  3.242, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[56],  3.108, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[57],  4.817, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[58],  7.066, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[59],  2.624, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[60],  6.588, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[61],  5.897, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[62],  5.883, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[63],  6.853, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[64],  9.626, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[65], 24.394, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[66],  3.478, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[67], 37.198, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[68], 14.271, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[69],  5.251, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[70], 11.269, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[71],  8.653, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[72], 23.695, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[73],  7.950, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[74],  6.610, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[75],  7.699, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[76],  3.969, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[77],  6.875, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[78], 16.782, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[79],  0.479, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[80],  7.676, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[81],  6.256, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[82],  7.829, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[83],  0.000, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[84],  0.000, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[85],  0.000, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[86],  0.000, TOLERATED_ERROR );
		TS_ASSERT_DELTA( scores[87],  0.000, TOLERATED_ERROR );
	}

	// failing example from Firas
	void test_sparta_foldit() {
		using core::Size;
		using core::Real;
		using std::string;
		using utility::vector1;
		using namespace protocols::sparta;
		float const TOLERATED_ERROR( 5e-3 );

		string const cs_file ( "protocols/sparta/data_17280.tab" );
		string const pdb_file( "protocols/sparta/solution_0022372568.ir_solution.pdb" );
		core::pose::Pose pose;
		core::import_pose::pose_from_pdb(pose,pdb_file);
		Sparta sparta(cs_file);

		Real const sparta_score(sparta.score_pose(pose));
		TS_ASSERT_DELTA( sparta_score, 702.378, TOLERATED_ERROR );
		std::cout << "score(" << pdb_file << "," << cs_file << ") = " << sparta_score <<
			std::endl;
	}

};
