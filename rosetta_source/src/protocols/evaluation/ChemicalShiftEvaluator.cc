// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file PoseEvaluator
/// @brief PoseEvaluator
/// @detailed
///
///
/// @author Oliver Lange



// Unit Headers
#include <protocols/evaluation/ChemicalShiftEvaluator.hh>

// Package Headers

// Project Headers
#include <core/io/silent/SilentStruct.hh>
#include <core/pose/Pose.hh>

// ObjexxFCL Headers

// Utility headers
#include <basic/Tracer.hh>
#include <basic/MemTracer.hh>

#include <utility/exit.hh>

//Auto Headers
#include <utility/options/keys/BooleanOptionKey.hh>

// C++ headers

//static basic::Tracer tr("protocols.evaluation.ChemicalShiftEvaluator");
using basic::mem_tr;

namespace protocols {
namespace evaluation {

using namespace core;
ChemicalShiftEvaluator::ChemicalShiftEvaluator( std::string tag, std::string cst_file ) :
	SingleValuePoseEvaluator<core::Real>( tag ),
	sparta_( cst_file )
{
	mem_tr << " Cstor-SPARTA Evaluator " << std::endl;
}

Real ChemicalShiftEvaluator::apply( pose::Pose& pose ) const {
	return sparta_.score_pose( pose );
}

bool ChemicalShiftEvaluator::applicable( pose::Pose const& pose ) const {
  return pose.is_fullatom();
}

}
}
