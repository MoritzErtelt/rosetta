// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/match/ScaffoldBuildPoint.cc
/// @brief  Class implementations for the launch point geometry on the Scaffold.
/// @author Alex Zanghellini (zanghell@u.washington.edu)
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com), porting to mini

// Unit headers
#include <protocols/match/upstream/ScaffoldBuildPoint.hh>

// Package headers
// AUTO-REMOVED #include <protocols/match/output/PoseInserter.hh>
#include <protocols/match/upstream/OriginalScaffoldBuildPoint.hh>

// AUTO-REMOVED #include <protocols/match/upstream/UpstreamBuilder.hh>

// Project headers
// AUTO-REMOVED #include <core/conformation/Residue.hh>
// AUTO-REMOVED #include <core/pack/dunbrack/SingleResidueDunbrackLibrary.hh>

// Numeric headers
// AUTO-REMOVED #include <numeric/HomogeneousTransform.hh>

namespace protocols {
namespace match {
namespace upstream {

ScaffoldBuildPoint::ScaffoldBuildPoint() : index_( 0 ) {}
ScaffoldBuildPoint::ScaffoldBuildPoint( Size index ) : index_( index ) {}
ScaffoldBuildPoint::~ScaffoldBuildPoint() {}

bool ScaffoldBuildPoint::compatible( ScaffoldBuildPoint const & other, bool first_dispatch ) const
{
	if ( ! first_dispatch ) {
		utility_exit_with_message( "CRITICAL ERROR: ScaffoldBuildPoint::compatible() double-dispatch failure" );
	}
	return other.compatible( *this, false );
}

bool ScaffoldBuildPoint::compatible( OriginalBackboneBuildPoint const & other, bool first_dispatch ) const
{
	if ( ! first_dispatch ) {
		utility_exit_with_message( "CRITICAL ERROR: ScaffoldBuildPoint::compatible() double-dispatch failure" );
	}
	return other.compatible( *this, false );
}


void
ScaffoldBuildPoint::index( Size setting ) {
	index_ = setting;
}



}
}
}

