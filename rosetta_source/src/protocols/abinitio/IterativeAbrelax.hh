// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file AbrelaxMover
/// @brief  this class will be handled to a SampleProtocol as a control instance
/// @detailed responsibilities:
///           know which chainbreaks to penalize and close
///           know which jumps to use during sampling, which (if any) to keep after loop-closing
///           supply a JumpMover if jumps should be moved
///           supply a MoveMap
///           supply a "StrictMoveMap": the protocol should not move anything that is dissallowed in strict_movemap(),
///                      it should try to move just stuff in movemap()
/// should this class also know how to ramp score terms ?
/// handle the titration of constraints ?
/// @author Oliver Lange


#ifndef INCLUDED_protocols_abinitio_IterativeAbrelax_hh
#define INCLUDED_protocols_abinitio_IterativeAbrelax_hh

// Unit Headers
//#include <protocols/abinitio/IterativeAbrelax.fwd.hh>

// Package Headers
#include <protocols/jd2/archive/ArchiveBase.hh>
#include <protocols/jd2/archive/EvaluatedArchive.hh>
#include <protocols/jd2/archive/ArchiveManager.fwd.hh>
#include <protocols/abinitio/IterativeCentroid.hh>
#include <protocols/abinitio/IterativeFullatom.hh>


// Project Headers
// AUTO-REMOVED #include <protocols/abinitio/PairingStatistics.hh>

#include <core/types.hh>
#include <core/pose/Pose.fwd.hh>

#include <protocols/loops/Loops.hh>

// ObjexxFCL Headers
//#include <ObjexxFCL/FArray1D.hh>
//#include <ObjexxFCL/FArray2D.hh>

// Utility headers
// AUTO-REMOVED #include <utility/vector1.hh>
#include <utility/pointer/ReferenceCount.hh>

//// C++ headers
#include <cstdlib>
#include <string>


namespace protocols {
namespace abinitio {

class IterativeAbrelax : public IterativeBase {
		//public jd2::archive::AbstractArchiveBase {
	typedef IterativeBase Parent;
	//AbstractArchiveBase Parent;
public:

  IterativeAbrelax( jd2::archive::ArchiveManagerAP ptr );

  virtual bool finished() const;
	//  virtual bool ready_for_batch() const;
	virtual bool still_interested( jd2::archive::Batch const& batch ) const;
  virtual void generate_batch();
  virtual void idle();

	static void register_options();

  //save Evaluator state ?
  virtual void save_to_file( std::string suffix = "" );
	virtual void save_status( std::ostream& ) const;
  virtual bool restore_from_file();
	virtual void init_from_decoy_set( core::io::silent::SilentFileData const& );
	virtual void read_structures( core::io::silent::SilentFileData&, jd2::archive::Batch const& batch );
	//	virtual void gen_evaluation_output( jd2::archive::Batch& batch, bool fullatom = false );

private:
	IterativeCentroid centroid_archive_;
	IterativeFullatom fullatom_archive_;
	bool fullatom_;
	static bool options_registered_;
};


}
}

#endif
