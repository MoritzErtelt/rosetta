// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is protocolsoped by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/protocols/UnfoldedStateEnergyCalculator/UnfoldedStateEnergyCalculatorMover.hh
/// @brief UnfoldedStateEnergyCalculatorMover class decalartion
/// @author P. douglas Renfrew (renfrew@unc.edu)

#ifndef INCLUDED_protocols_unfolded_state_energy_calculator_UnfoldedStateEnergyCalculatorMover_hh
#define INCLUDED_protocols_unfolded_state_energy_calculator_UnfoldedStateEnergyCalculatorMover_hh

// Unit Headers
#include <protocols/unfolded_state_energy_calculator/UnfoldedStateEnergyCalculatorMover.fwd.hh>
#include <protocols/unfolded_state_energy_calculator/UnfoldedStateEnergyCalculatorJobDistributor.hh>
#include <protocols/unfolded_state_energy_calculator/UnfoldedStateEnergyCalculatorMPIWorkPoolJobDistributor.hh>

// Package headers
#include <protocols/moves/Mover.hh>

// Project headers
#include <core/types.hh>
#include <core/pose/Pose.fwd.hh>

#include <core/scoring/ScoreFunction.fwd.hh>

// Utility Headers

// C++ Headers


namespace protocols {
namespace unfolded_state_energy_calculator {

/// @brief
class UnfoldedStateEnergyCalculatorMover : public protocols::moves::Mover {

// ctors, dtors, and cctors
public:

	///@brief ctor
	UnfoldedStateEnergyCalculatorMover(
#ifdef USEMPI
		UnfoldedStateEnergyCalculatorMPIWorkPoolJobDistributor & job_dist,
#else
		UnfoldedStateEnergyCalculatorJobDistributor & job_dist,
#endif
		core::scoring::ScoreFunctionCOP pack_scrfxn,
		core::scoring::ScoreFunctionCOP score_scrfxn,
		core::Size frag_length,
		std::string mut_aa,
		bool repack_fragments,
		bool native_sequence
	);

  ///@brief dtor
  virtual ~UnfoldedStateEnergyCalculatorMover();

  ///@brief cctor
  UnfoldedStateEnergyCalculatorMover( UnfoldedStateEnergyCalculatorMover const & usecm );


// mover and job distributor interface functions
public:

	///@brief
	virtual protocols::moves::MoverOP fresh_instance() const;

	///@brief
	virtual void apply( Pose & pose );

	virtual std::string get_name() const;

	///@brief
	virtual	bool reinitialize_for_each_job() const;

	///@brief
	virtual	bool reinitialize_for_new_input() const;

// class specific functions
public:

// data
private:

	// job distributor
#ifdef USEMPI
	UnfoldedStateEnergyCalculatorMPIWorkPoolJobDistributor & job_dist_;
#else
	UnfoldedStateEnergyCalculatorJobDistributor & job_dist_;
#endif

	// score function to use when packing fragments
	core::scoring::ScoreFunctionCOP pack_scrfxn_;

	// score function to use when evaluating fragments
	core::scoring::ScoreFunctionCOP score_scrfxn_;

	// the number of residues in each fragment
	core::Size frag_length_;

	// the AA to mutate the central residue to
	std::string mut_aa_;

	// will the fragments be repacked before being scored
	bool repack_fragments_;

	// will the central residue be mutated before being scored
	bool native_sequence_;

};

} // UnfoldedStateEnergyCalculator
} // protocols

#endif //INCLUDED_protocols_UnfoldedStateEnergyCalculator_UnfoldedStateEnergyCalculatorMover_HH
