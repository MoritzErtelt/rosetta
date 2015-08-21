// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/mm/MMTorsionScore.cxxtest.hh
/// @brief  test suite for core::scoring::mm::MMTorsionScore
/// @author P. Douglas Renfrew (renfrew@nyu.edu)


// Test headers
#include <cxxtest/TestSuite.h>

// Unit headers
#include <core/scoring/mm/MMTorsionScore.hh>
#include <core/types.hh>

// Project headers
#include <test/core/init_util.hh>

// Utility headers
#include <utility/pointer/owning_ptr.hh>

// Numeric headers
#include <numeric/conversions.hh>

// C++ headers
#include <string>

//Auto Headers
#include <core/chemical/MMAtomTypeSet.hh>
#include <utility/keys/Key3Tuple.hh>
#include <utility/keys/Key4Tuple.hh>


using namespace core;
using namespace core::scoring::mm;
using namespace core::chemical;

// --------------- Test Class --------------- //

class MMTorsionScoreTests : public CxxTest::TestSuite {

public:

	MMTorsionLibraryOP mmtorsionlibrary;
	MMAtomTypeSetOP mmatomtypeset;
	MMTorsionScoreOP mmtorsionscore;
	double delta;

	// --------------- Suite-level Fixture --------------- //

	MMTorsionScoreTests() {
		core_init();

		// Only want to read in copy of the library once not for each test so init here in ctor

		// init the mmatomtypeset
		mmatomtypeset = MMAtomTypeSetOP( new MMAtomTypeSet );
		mmatomtypeset->read_file( "core/chemical/mm_atom_properties.txt" );

		// init the mmtorsionlibrary
		mmtorsionlibrary = MMTorsionLibraryOP( new MMTorsionLibrary( "core/scoring/mm/mm_torsion_params.txt" , mmatomtypeset ) );

		// init the mmtorsionscore
		mmtorsionscore = MMTorsionScoreOP( new MMTorsionScore( *mmtorsionlibrary ) );
	}

	virtual ~MMTorsionScoreTests() {}

	static MMTorsionScoreTests* createSuite() {
		return new MMTorsionScoreTests();
	}

	static void destroySuite( MMTorsionScoreTests *suite ) {
		delete suite;
	}

	// --------------- Fixtures --------------- //

	void setUp() {
		delta = 0.00000001;
	}

	void tearDown() { }

	// ------------- Helper Function ------------- //

	mm_torsion_atom_quad make_quad( std::string s1,  std::string s2, std::string s3, std::string s4 ) {
		return mm_torsion_atom_quad( mmatomtypeset->atom_type_index( s1 ),
			mmatomtypeset->atom_type_index( s2 ),
			mmatomtypeset->atom_type_index( s3 ),
			mmatomtypeset->atom_type_index( s4 ) );
	}

	// --------------- Test Cases --------------- //

	void test_score() {

		// make set of angles in radians
		Real angles[360];
		for ( int i = 0; i<360; ++i ) {
			angles[i] = numeric::conversions::radians( static_cast<Real>( i ) );
		}

		// pre-calculated scores
		Real A_scores[] = { 0.00000000, 0.00003046, 0.00012183, 0.00027409, 0.00048719, 0.00076106, 0.00109562, 0.00149077, 0.00194639, 0.00246233,
			0.00303845, 0.00367456, 0.00437048, 0.00512599, 0.00594085, 0.00681483, 0.00774766, 0.00873905, 0.00978870, 0.01089628,
			0.01206148, 0.01328391, 0.01456323, 0.01589903, 0.01729091, 0.01873844, 0.02024119, 0.02179870, 0.02341048, 0.02507606,
			0.02679492, 0.02856654, 0.03039038, 0.03226589, 0.03419249, 0.03616959, 0.03819660, 0.04027290, 0.04239785, 0.04457081,
			0.04679111, 0.04905808, 0.05137103, 0.05372926, 0.05613204, 0.05857864, 0.06106833, 0.06360033, 0.06617388, 0.06878819,
			0.07144248, 0.07413592, 0.07686770, 0.07963700, 0.08244295, 0.08528471, 0.08816142, 0.09107219, 0.09401615, 0.09699239,
			0.10000000, 0.10303808, 0.10610569, 0.10920190, 0.11232577, 0.11547635, 0.11865267, 0.12185377, 0.12507868, 0.12832641,
			0.13159597, 0.13488637, 0.13819660, 0.14152566, 0.14487253, 0.14823619, 0.15161562, 0.15500979, 0.15841766, 0.16183820,
			0.16527036, 0.16871311, 0.17216538, 0.17562613, 0.17909431, 0.18256885, 0.18604871, 0.18953281, 0.19302010, 0.19650952,
			0.20000000, 0.20349048, 0.20697990, 0.21046719, 0.21395129, 0.21743115, 0.22090569, 0.22437387, 0.22783462, 0.23128689,
			0.23472964, 0.23816180, 0.24158234, 0.24499021, 0.24838438, 0.25176381, 0.25512747, 0.25847434, 0.26180340, 0.26511363,
			0.26840403, 0.27167359, 0.27492132, 0.27814623, 0.28134733, 0.28452365, 0.28767423, 0.29079810, 0.29389431, 0.29696192,
			0.30000000, 0.30300761, 0.30598385, 0.30892781, 0.31183858, 0.31471529, 0.31755705, 0.32036300, 0.32313230, 0.32586408,
			0.32855752, 0.33121181, 0.33382612, 0.33639967, 0.33893167, 0.34142136, 0.34386796, 0.34627074, 0.34862897, 0.35094192,
			0.35320889, 0.35542919, 0.35760215, 0.35972710, 0.36180340, 0.36383041, 0.36580751, 0.36773411, 0.36960962, 0.37143346,
			0.37320508, 0.37492394, 0.37658952, 0.37820130, 0.37975881, 0.38126156, 0.38270909, 0.38410097, 0.38543677, 0.38671609,
			0.38793852, 0.38910372, 0.39021130, 0.39126095, 0.39225234, 0.39318517, 0.39405915, 0.39487401, 0.39562952, 0.39632544,
			0.39696155, 0.39753767, 0.39805361, 0.39850923, 0.39890438, 0.39923894, 0.39951281, 0.39972591, 0.39987817, 0.39996954,
			0.40000000, 0.39996954, 0.39987817, 0.39972591, 0.39951281, 0.39923894, 0.39890438, 0.39850923, 0.39805361, 0.39753767,
			0.39696155, 0.39632544, 0.39562952, 0.39487401, 0.39405915, 0.39318517, 0.39225234, 0.39126095, 0.39021130, 0.38910372,
			0.38793852, 0.38671609, 0.38543677, 0.38410097, 0.38270909, 0.38126156, 0.37975881, 0.37820130, 0.37658952, 0.37492394,
			0.37320508, 0.37143346, 0.36960962, 0.36773411, 0.36580751, 0.36383041, 0.36180340, 0.35972710, 0.35760215, 0.35542919,
			0.35320889, 0.35094192, 0.34862897, 0.34627074, 0.34386796, 0.34142136, 0.33893167, 0.33639967, 0.33382612, 0.33121181,
			0.32855752, 0.32586408, 0.32313230, 0.32036300, 0.31755705, 0.31471529, 0.31183858, 0.30892781, 0.30598385, 0.30300761,
			0.30000000, 0.29696192, 0.29389431, 0.29079810, 0.28767423, 0.28452365, 0.28134733, 0.27814623, 0.27492132, 0.27167359,
			0.26840403, 0.26511363, 0.26180340, 0.25847434, 0.25512747, 0.25176381, 0.24838438, 0.24499021, 0.24158234, 0.23816180,
			0.23472964, 0.23128689, 0.22783462, 0.22437387, 0.22090569, 0.21743115, 0.21395129, 0.21046719, 0.20697990, 0.20349048,
			0.20000000, 0.19650952, 0.19302010, 0.18953281, 0.18604871, 0.18256885, 0.17909431, 0.17562613, 0.17216538, 0.16871311,
			0.16527036, 0.16183820, 0.15841766, 0.15500979, 0.15161562, 0.14823619, 0.14487253, 0.14152566, 0.13819660, 0.13488637,
			0.13159597, 0.12832641, 0.12507868, 0.12185377, 0.11865267, 0.11547635, 0.11232577, 0.10920190, 0.10610569, 0.10303808,
			0.10000000, 0.09699239, 0.09401615, 0.09107219, 0.08816142, 0.08528471, 0.08244295, 0.07963700, 0.07686770, 0.07413592,
			0.07144248, 0.06878819, 0.06617388, 0.06360033, 0.06106833, 0.05857864, 0.05613204, 0.05372926, 0.05137103, 0.04905808,
			0.04679111, 0.04457081, 0.04239785, 0.04027290, 0.03819660, 0.03616959, 0.03419249, 0.03226589, 0.03039038, 0.02856654,
			0.02679492, 0.02507606, 0.02341048, 0.02179870, 0.02024119, 0.01873844, 0.01729091, 0.01589903, 0.01456323, 0.01328391,
			0.01206148, 0.01089628, 0.00978870, 0.00873905, 0.00774766, 0.00681483, 0.00594085, 0.00512599, 0.00437048, 0.00367456,
			0.00303845, 0.00246233, 0.00194639, 0.00149077, 0.00109562, 0.00076106, 0.00048719, 0.00027409, 0.00012183, 0.00003046 };

		// compare calculated score with precalculated score
		mm_torsion_atom_quad A = make_quad( "C", "CA", "CC", "CP1" );
		for ( int i = 0; i<360; ++i ) {
			TS_ASSERT_DELTA( mmtorsionscore->score( A, angles[i] ), A_scores[i], delta );
		}

		// pre-calculated scores for multiparam
		Real B_scores[] = { 0.00000000, 0.00042639, 0.00170465, 0.00383205, 0.00680406, 0.01061434, 0.01525480, 0.02071554, 0.02698496, 0.03404972,
			0.04189484, 0.05050368, 0.05985799, 0.06993799, 0.08072237, 0.09218840, 0.10431192, 0.11706746, 0.13042825, 0.14436633,
			0.15885259, 0.17385685, 0.18934794, 0.20529377, 0.22166139, 0.23841711, 0.25552656, 0.27295475, 0.29066621, 0.30862501,
			0.32679492, 0.34513942, 0.36362184, 0.38220545, 0.40085350, 0.41952937, 0.43819660, 0.45681902, 0.47536080, 0.49378657,
			0.51206148, 0.53015127, 0.54802239, 0.56564204, 0.58297826, 0.60000000, 0.61667719, 0.63298082, 0.64888297, 0.66435693,
			0.67937719, 0.69391956, 0.70796118, 0.72148055, 0.73445765, 0.74687391, 0.75871226, 0.76995719, 0.78059476, 0.79061260,
			0.80000000, 0.80874784, 0.81684865, 0.82429662, 0.83108759, 0.83721903, 0.84269010, 0.84750153, 0.85165573, 0.85515668,
			0.85800994, 0.86022263, 0.86180340, 0.86276237, 0.86311111, 0.86286263, 0.86203126, 0.86063268, 0.85868380, 0.85620278,
			0.85320889, 0.84972251, 0.84576505, 0.84135887, 0.83652723, 0.83129421, 0.82568466, 0.81972408, 0.81343860, 0.80685488,
			0.80000000, 0.79290146, 0.78558702, 0.77808468, 0.77042257, 0.76262889, 0.75473181, 0.74675942, 0.73873963, 0.73070010,
			0.72266816, 0.71467076, 0.70673438, 0.69888494, 0.69114778, 0.68354753, 0.67610813, 0.66885266, 0.66180340, 0.65498167,
			0.64840784, 0.64210125, 0.63608019, 0.63036181, 0.62496215, 0.61989601, 0.61517700, 0.61081748, 0.60682851, 0.60321987,
			0.60000000, 0.59717602, 0.59475370, 0.59273747, 0.59113038, 0.58993415, 0.58914915, 0.58877439, 0.58880758, 0.58924511,
			0.59008208, 0.59131231, 0.59292842, 0.59492177, 0.59728261, 0.60000000, 0.60306194, 0.60645537, 0.61016622, 0.61417949,
			0.61847925, 0.62304875, 0.62787044, 0.63292604, 0.63819660, 0.64366257, 0.64930386, 0.65509989, 0.66102970, 0.66707196,
			0.67320508, 0.67940728, 0.68565663, 0.69193115, 0.69820885, 0.70446784, 0.71068637, 0.71684289, 0.72291614, 0.72888522,
			0.73472964, 0.74042937, 0.74596495, 0.75131751, 0.75646884, 0.76140144, 0.76609859, 0.77054440, 0.77472383, 0.77862278,
			0.78222811, 0.78552767, 0.78851037, 0.79116617, 0.79348616, 0.79546255, 0.79708872, 0.79835920, 0.79926973, 0.79981728,
			0.80000000, 0.79981728, 0.79926973, 0.79835920, 0.79708872, 0.79546255, 0.79348616, 0.79116617, 0.78851037, 0.78552767,
			0.78222811, 0.77862278, 0.77472383, 0.77054440, 0.76609859, 0.76140144, 0.75646884, 0.75131751, 0.74596495, 0.74042937,
			0.73472964, 0.72888522, 0.72291614, 0.71684289, 0.71068637, 0.70446784, 0.69820885, 0.69193115, 0.68565663, 0.67940728,
			0.67320508, 0.66707196, 0.66102970, 0.65509989, 0.64930386, 0.64366257, 0.63819660, 0.63292604, 0.62787044, 0.62304875,
			0.61847925, 0.61417949, 0.61016622, 0.60645537, 0.60306194, 0.60000000, 0.59728261, 0.59492177, 0.59292842, 0.59131231,
			0.59008208, 0.58924511, 0.58880758, 0.58877439, 0.58914915, 0.58993415, 0.59113038, 0.59273747, 0.59475370, 0.59717602,
			0.60000000, 0.60321987, 0.60682851, 0.61081748, 0.61517700, 0.61989601, 0.62496215, 0.63036181, 0.63608019, 0.64210125,
			0.64840784, 0.65498167, 0.66180340, 0.66885266, 0.67610813, 0.68354753, 0.69114778, 0.69888494, 0.70673438, 0.71467076,
			0.72266816, 0.73070010, 0.73873963, 0.74675942, 0.75473181, 0.76262889, 0.77042257, 0.77808468, 0.78558702, 0.79290146,
			0.80000000, 0.80685488, 0.81343860, 0.81972408, 0.82568466, 0.83129421, 0.83652723, 0.84135887, 0.84576505, 0.84972251,
			0.85320889, 0.85620278, 0.85868380, 0.86063268, 0.86203126, 0.86286263, 0.86311111, 0.86276237, 0.86180340, 0.86022263,
			0.85800994, 0.85515668, 0.85165573, 0.84750153, 0.84269010, 0.83721903, 0.83108759, 0.82429662, 0.81684865, 0.80874784,
			0.80000000, 0.79061260, 0.78059476, 0.76995719, 0.75871226, 0.74687391, 0.73445765, 0.72148055, 0.70796118, 0.69391956,
			0.67937719, 0.66435693, 0.64888297, 0.63298082, 0.61667719, 0.60000000, 0.58297826, 0.56564204, 0.54802239, 0.53015127,
			0.51206148, 0.49378657, 0.47536080, 0.45681902, 0.43819660, 0.41952937, 0.40085350, 0.38220545, 0.36362184, 0.34513942,
			0.32679492, 0.30862501, 0.29066621, 0.27295475, 0.25552656, 0.23841711, 0.22166139, 0.20529377, 0.18934794, 0.17385685,
			0.15885259, 0.14436633, 0.13042825, 0.11706746, 0.10431192, 0.09218840, 0.08072237, 0.06993799, 0.05985799, 0.05050368,
			0.04189484, 0.03404972, 0.02698496, 0.02071554, 0.01525480, 0.01061434, 0.00680406, 0.00383205, 0.00170465, 0.00042639 };

		// compare calculated score with precalculated score
		mm_torsion_atom_quad B = make_quad( "CPH1", "CP3", "CC", "C" );
		for (  int i = 0; i<360; ++i ) {
			TS_ASSERT_DELTA( mmtorsionscore->score( B, angles[i] ), B_scores[i], delta );
		}
	}
};
