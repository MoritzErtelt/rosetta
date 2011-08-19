// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/loophash/LoopHashSampler.hh
/// @brief
/// @author Mike Tyka



#ifndef INCLUDED_protocols_loophash_LoopHashSampler_hh
#define INCLUDED_protocols_loophash_LoopHashSampler_hh

#include <protocols/loophash/LoopHashSampler.fwd.hh>
#include <protocols/loophash/LoopHashLibrary.fwd.hh>
#include <protocols/loophash/LocalInserter.fwd.hh>

#include <core/types.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/io/silent/SilentStruct.fwd.hh>
#include <string>
#include <vector>

namespace protocols {
namespace loophash {



class LoopHashSampler {
  public:

  LoopHashSampler(
    LoopHashLibraryOP library,
    LocalInserterOP inserter
	);

	~LoopHashSampler();

	void set_defaults();

  /// @brief create a set of structures for a the given range of residues and other parameters stored int his class.
  void build_structures(
		const core::pose::Pose& start_pose,
    std::vector< core::io::silent::SilentStructOP > &lib_structs
	);


  void set_start_res( core::Size  value ) {  start_res_  = value; }
  void set_stop_res ( core::Size  value ) {  stop_res_   = value; }
  void set_min_bbrms( core::Real  value ) {  min_bbrms_  = value; }
  void set_max_bbrms( core::Real  value ) {  max_bbrms_  = value; }
  void set_min_rms  ( core::Real  value ) {  min_rms_    = value; }
  void set_max_rms  ( core::Real  value ) {  max_rms_    = value; }
  void set_max_nstruct  ( core::Size  value ) {  max_nstruct_    = value; }
  void set_nonideal  ( bool value ) {  nonideal_  = value; }

  core::Size get_start_res() { return  start_res_ ; }
  core::Size get_stop_res () { return  stop_res_  ; }
  core::Real get_min_bbrms() { return  min_bbrms_ ; }
  core::Real get_max_bbrms() { return  max_bbrms_ ; }
  core::Real get_min_rms  () { return  min_rms_   ; }
  core::Real get_max_rms  () { return  max_rms_   ; }
  core::Size get_max_nstruct() { return  max_nstruct_; }

	//fpd pre-filter structures with a scorefunction
	//fpd   this is done using a chainbroken pose (before constraint minimization!)
	//fpd useful for experimentally derived scorefunctions (eg density)
	void use_prefiltering( core::scoring::ScoreFunctionOP score_filt, core::Size nstruct ) {
		score_filt_ = score_filt;
		nprefilter_ = nstruct;
	}

  private:
    /// @brief pointer to the library used for insertion
    LoopHashLibraryOP library_;

    /// @brief pointer to the insertion functor which provides the peptide insertion facility
    LocalInserterOP inserter_;

    /// @brief parameters for insertion positions
    core::Size start_res_;
    core::Size stop_res_ ;
    core::Real min_bbrms_;
		core::Real max_bbrms_;
		core::Real min_rms_  ;
		core::Real max_rms_  ;
		core::Size max_nstruct_;
		bool nonideal_;

		/// @brief (fpd) pre-filtering options
		core::Size nprefilter_;
		core::scoring::ScoreFunctionOP score_filt_;
};





}
}

#endif
