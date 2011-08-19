// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file ConformerSwitchMover.cc
/// @brief code for the conformer switch mover in ensemble docking
/// @author Sid Chaudhury

#ifndef INCLUDED_protocols_moves_ConformerSwitchMover_hh
#define INCLUDED_protocols_moves_ConformerSwitchMover_hh

// Unit headers
#include <protocols/moves/ConformerSwitchMover.fwd.hh>

// Package headers
#include <protocols/moves/Mover.hh>
#include <protocols/docking/DockingEnsemble.fwd.hh>
#include <protocols/docking/DockFilters.hh>

#include <core/types.hh>

#include <core/pose/Pose.hh>
#include <core/scoring/ScoreFunction.hh>

//Option key includes

// ObjexxFCL Headers

// C++ Headers
#include <map>
#include <string>

// Utility Headers
#include <utility/pointer/ReferenceCount.hh>

//Auto Headers
#include <utility/options/keys/BooleanOptionKey.hh>


namespace protocols {
namespace moves {

/// @brief this mover does the conformer swap in RosettaDock's ensemble docking.  It takes
/// in a multi-model PDB file as an ensemble, and does swaps conformers by superpositioning
/// over interface residues, and selects a conformer based on a partition function using
/// a ScoreFunction.
class ConformerSwitchMover : public Mover {

public:

	//default constructor
	ConformerSwitchMover();

	//constructor with arguments
	ConformerSwitchMover(
		protocols::docking::DockingEnsembleOP ensemble,
		bool random_conformer=false
	);

	virtual void apply( core::pose::Pose & pose );

	void SwitchConformer(
		core::pose::Pose & pose,
		core::Size conf_num
	);

	virtual std::string get_name() const;
	void set_temperature( core::Real temp_in );

	void set_random_confomer( bool rand ) { random_conformer_ = rand; }

private:

	void GenerateProbTable( core::pose::Pose & pose );

	bool random_conformer_;
	core::Real temperature_;
	utility::vector1< core::Real > prob_table_;
	protocols::docking::DockingEnsembleOP ensemble_;
	protocols::docking::DockingLowResFilterOP lowres_filter_;
}; //mover

} // moves
} // rosetta


#endif
