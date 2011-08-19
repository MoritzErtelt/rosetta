// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   numeric/xyz.functions.cxxtest.hh
/// @brief  test suite for numeric::xyz_functions
/// @author Stuart G. Mentzer (Stuart_Mentzer@objexx.com)
/// @author Kevin P. Hinshaw (KevinHinshaw@gmail.com)
/// @author Ron Jacak (ron.jacak@gmail.com)


// Test headers
#include <cxxtest/TestSuite.h>

// Package headers
#include <numeric/xyz.functions.hh>
#include <numeric/xyzVector.hh>
#include <numeric/sphericalVector.hh>
#include <numeric/xyzVector.io.hh>
#include <numeric/xyzMatrix.hh>
#include <numeric/xyzMatrix.io.hh>
#include <numeric/constants.hh>


// --------------- Test Class --------------- //

class XYZFunctionsTests : public CxxTest::TestSuite {

	public:

	// Shared data elements go here.
	double delta_percent;         // percentage difference for floating-point comparisons in TS_ASSERT_DELTA

	// --------------- Fixtures --------------- //

	// Define a test fixture (some initial state that several tests share)
	// In CxxTest, setUp()/tearDown() are executed around each test case. If you need a fixture on the test
	// suite level, i.e. something that gets constructed once before all the tests in the test suite are run,
	// suites have to be dynamically created. See CxxTest sample directory for example.

	// Shared initialization goes here.
	void setUp() {
		delta_percent = 0.0001;
	}

	// Shared finalization goes here.
	void tearDown() {
	}


	// --------------- Test Cases --------------- //
	/// @brief operations tests
	void test_xyz_functions_Operations() {

		using numeric::xyzMatrix_float;
		using numeric::xyzVector_float;

		// compute an outer product
		xyzVector_float v( 1.0, 2.0, 3.0 );
		xyzVector_float w( 2.0, 1.0, 5.0 );
		xyzMatrix_float m = outer_product( v, w );
		TS_ASSERT_EQUALS( m, xyzMatrix_float::rows( 2.0, 1.0, 5.0, 4.0, 2.0, 10.0, 6.0, 3.0, 15.0 ) ); // equality here

		// matrix/vector product
		xyzVector_float u1 = m * v;
		TS_ASSERT_EQUALS( u1, xyzVector_float( 19.0, 38.0, 57.0 ) ); // equality here

		// matrix/vector in-place product
		xyzVector_float u2 = inplace_product( m, v );
		TS_ASSERT_EQUALS( u2, xyzVector_float( 19.0, 38.0, 57.0 ) ); // equality here
	}


	/// @brief test eigenvalue/eigenvector computations
	void  test_xyz_functions_Eigenvalues() {

		using numeric::xyzMatrix_double;
		using numeric::xyzVector_double;

		// build a symmetric matrix from three vectors
		xyzVector_double v1( 1.8, 2.0, 4.0 );
		xyzVector_double v2( 2.0, 5.0, 7.0 );
		xyzVector_double v3( 4.0, 7.0, 9.0 );
		xyzMatrix_double m1 = xyzMatrix_double::rows( v1, v2, v3 );

		// compute eigenvalues
		xyzVector_double ev1 = eigenvalue_jacobi( m1, 0.0 );
	//	std::cout << "m1:\n" << m1 << std::endl;
	//	std::cout << "eigenvalue_Jacobi returned: \n" << ev1 << std::endl;
		TS_ASSERT_DELTA( ev1.x(),  0.9084668081160422, delta_percent );
		TS_ASSERT_DELTA( ev1.y(), -0.7863597288923477, delta_percent );
		TS_ASSERT_DELTA( ev1.z(), 15.67789292077631  , delta_percent );

		// compute eigenvectors
		xyzMatrix_double ew1;
		ev1 = eigenvector_jacobi( m1, 0.0, ew1 );
	//	std::cout << "Eigenvalues from eigenvector_Jacobi:\n" << ev1 << std::endl;
		TS_ASSERT_DELTA( ev1.x(),  0.9084668081160422, delta_percent );
		TS_ASSERT_DELTA( ev1.y(), -0.7863597288923477, delta_percent );
		TS_ASSERT_DELTA( ev1.z(), 15.67789292077631  , delta_percent );
	//	std::cout << "Eigenvectors:\n" << ew1 << std::endl;
		TS_ASSERT_DELTA( ew1.xx(),  0.7968224391550893, delta_percent );
		TS_ASSERT_DELTA( ew1.xy(),  0.5228228506830058, delta_percent );
		TS_ASSERT_DELTA( ew1.xz(),  0.3028700501248519, delta_percent );
		TS_ASSERT_DELTA( ew1.yx(), -0.5924536292558642, delta_percent );
		TS_ASSERT_DELTA( ew1.yy(),  0.5776511119658608, delta_percent );
		TS_ASSERT_DELTA( ew1.yz(),  0.5615317355467631, delta_percent );
		TS_ASSERT_DELTA( ew1.zx(),  0.1186284014917577, delta_percent );
		TS_ASSERT_DELTA( ew1.zy(), -0.6268775475707361, delta_percent );
		TS_ASSERT_DELTA( ew1.zz(),  0.7700336633623303, delta_percent );

		// compute product of eigenvectors and a diagonal matrix of eigenvalues
		xyzMatrix_double diag = xyzMatrix_double::diag( ev1.x(), ev1.y(), ev1.z() );
		xyzMatrix_double ew1_diag = ew1 * diag;
	//	std::cout << "Eigenvectors * diagonal:\n" << ew1_diag  << std::endl;
		TS_ASSERT_DELTA( ew1_diag.xx(),  0.7238867379344632, delta_percent );
		TS_ASSERT_DELTA( ew1_diag.xy(), -0.4111268351218129, delta_percent );
		TS_ASSERT_DELTA( ew1_diag.xz(),  4.748364214767583 , delta_percent );
		TS_ASSERT_DELTA( ew1_diag.yx(), -0.5382244575268400, delta_percent );
		TS_ASSERT_DELTA( ew1_diag.yy(), -0.4542415717998375, delta_percent );
		TS_ASSERT_DELTA( ew1_diag.yz(),  8.803634421519833 , delta_percent );
		TS_ASSERT_DELTA( ew1_diag.zx(),  0.1077699652551254, delta_percent );
		TS_ASSERT_DELTA( ew1_diag.zy(),  0.4929512583564238, delta_percent );
		TS_ASSERT_DELTA( ew1_diag.zz(),  12.07250531958773 , delta_percent );

		// compute product of original matrix and eigenvectors
		xyzMatrix_double m1_ew1 = m1 * ew1;
	//	std::cout << "m1 * Eigenvectors:\n" << m1_ew1 << std::endl;
		TS_ASSERT_DELTA( m1_ew1.xx(),  0.7238867379344631, delta_percent );
		TS_ASSERT_DELTA( m1_ew1.xy(), -0.4111268351218125, delta_percent );
		TS_ASSERT_DELTA( m1_ew1.xz(),  4.748364214767580 , delta_percent );
		TS_ASSERT_DELTA( m1_ew1.yx(), -0.5382244575268387, delta_percent );
		TS_ASSERT_DELTA( m1_ew1.yy(), -0.4542415717998374, delta_percent );
		TS_ASSERT_DELTA( m1_ew1.yz(),  8.803634421519831 , delta_percent );
		TS_ASSERT_DELTA( m1_ew1.zx(),  0.1077699652551269, delta_percent );
		TS_ASSERT_DELTA( m1_ew1.zy(),  0.4929512583564248, delta_percent );
		TS_ASSERT_DELTA( m1_ew1.zz(), 12.07250531958772  , delta_percent );

		// the two products above should match
		TS_ASSERT_DELTA( m1_ew1.xx(), ew1_diag.xx(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.xy(), ew1_diag.xy(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.xz(), ew1_diag.xz(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.yx(), ew1_diag.yx(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.yy(), ew1_diag.yy(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.yz(), ew1_diag.yz(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.zx(), ew1_diag.zx(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.zy(), ew1_diag.zy(), delta_percent );
		TS_ASSERT_DELTA( m1_ew1.zz(), ew1_diag.zz(), delta_percent );
	}


	/// @brief test matrix rotation
	void  test_xyz_functions_Rotation() {

		using namespace numeric::constants::d;
		using numeric::xyzMatrix_double;
		using numeric::xyzVector_double;

		// compute a rotation matrix, given an axis/angle pair
		xyzVector_double axis1( -4.0, 2.0, -1.0 );
		double theta1 = 45.0 * degrees_to_radians;
		xyzMatrix_double rm = rotation_matrix( axis1, theta1 );
		TS_ASSERT_DELTA( rm.xx(),  0.9302635193301304 , delta_percent );
		TS_ASSERT_DELTA( rm.xy(),  0.04272498089030048, delta_percent );
		TS_ASSERT_DELTA( rm.xz(),  0.3643958844600796 , delta_percent );
		TS_ASSERT_DELTA( rm.yx(), -0.2658817190338834 , delta_percent );
		TS_ASSERT_DELTA( rm.yy(),  0.7628959657224432 , delta_percent );
		TS_ASSERT_DELTA( rm.yz(),  0.5893188075804199 , delta_percent );
		TS_ASSERT_DELTA( rm.zx(), -0.2528175153882882 , delta_percent );
		TS_ASSERT_DELTA( rm.zy(), -0.6451079921163156 , delta_percent );
		TS_ASSERT_DELTA( rm.zz(),  0.7210540773205214 , delta_percent );

		// given the rotation matrix, retrieve the axis/angle rotation pair
		double theta2 = 0.0;
		xyzVector_double axis2 = rotation_axis( rm, theta2 );  // modifies theta2

		// the rotation angle should match the original
		TS_ASSERT_DELTA( theta1, theta2, delta_percent );

		// the rotation axis should match a normalized version of the original
		xyzVector_double axis1_norm = axis1.normalized();
		TS_ASSERT_DELTA( axis1_norm.x(), axis2.x(), delta_percent );
		TS_ASSERT_DELTA( axis1_norm.y(), axis2.y(), delta_percent );
		TS_ASSERT_DELTA( axis1_norm.z(), axis2.z(), delta_percent );
	}


	/// @brief angle tests
	void test_xyz_functions_AngleOperations() {

		using numeric::xyzVector_float;
		using numeric::dihedral;

		// compute a dihedral angle
		{
		xyzVector_float p1( -16.577999, 5.2760000, -10.507000 );
		xyzVector_float p2( -16.726999, 6.5549998, -10.734000 );
		xyzVector_float p3( -17.601999, 7.3889999, -9.8529997 );
		xyzVector_float p4( -16.798000, 7.8550000, -8.6330004 );
		TS_ASSERT_DELTA( dihedral( p1,  p2,  p3,  p4 ), 85.0868f, delta_percent );
		}

		// compute another dihedral angle
		{
		xyzVector_float p1( -2.5362606, 15.318624, 30.642799 );
		xyzVector_float p2( -2.6063213, 15.113583, 32.148636 );
		xyzVector_float p3( -2.8367538, 16.429384, 32.877182 );
		xyzVector_float p4( -2.9080868, 16.243940, 34.351967 );

		// test normal case
		TS_ASSERT_DELTA( dihedral( p1,  p2,  p3,  p4 ), 179.9761f, delta_percent );

		// test degenerate cases (angle should be zero)
		TS_ASSERT_DELTA( dihedral( p1,  p1,  p3,  p4 ), 0.0f, delta_percent );
		TS_ASSERT_DELTA( dihedral( p1,  p2,  p3,  p2 ), 0.0f, delta_percent );
		}
	}

	void test_xyz_functions_SphericalConversions()
	{
		numeric::xyzVector<float> xyz(1,1,1);
		numeric::sphericalVector<float> spherical(numeric::xyz_to_spherical<float>(xyz));
		TS_ASSERT_DELTA(spherical.radius(),1.7320f,delta_percent);
		TS_ASSERT_DELTA(spherical.theta(),54.7356f,delta_percent);
		TS_ASSERT_DELTA(spherical.phi(),45.0000f,delta_percent);

		numeric::xyzVector<float> new_xyz(numeric::spherical_to_xyz<float>(spherical));
		TS_ASSERT_DELTA(new_xyz.x(),1.0000f,delta_percent);
		TS_ASSERT_DELTA(new_xyz.y(),1.0000f,delta_percent);
		TS_ASSERT_DELTA(new_xyz.z(),1.0000f,delta_percent);
	}

};


