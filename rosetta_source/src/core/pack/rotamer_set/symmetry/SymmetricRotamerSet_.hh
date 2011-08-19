// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file   core/pack/rotamer_set/symmetry/SymmetricRotamerSet_.hh
/// @brief  rotamer set implementation class for symmetric packing
/// @author Ingemar Andre


#ifndef INCLUDED_core_pack_rotamer_set_symmetry_SymmetricRotamerSet__hh
#define INCLUDED_core_pack_rotamer_set_symmetry_SymmetricRotamerSet__hh

//Unit headers
#include <core/pack/rotamer_set/symmetry/SymmetricRotamerSet_.fwd.hh>

//Package headers
#include <core/pack/rotamer_set/RotamerSet_.hh>
#include <core/pack/rotamer_set/BumpSelector.hh>

//Project headers
#include <core/conformation/Residue.fwd.hh>
#include <core/chemical/ResidueType.fwd.hh>
#include <core/pack/dunbrack/RotamerLibrary.fwd.hh>
// AUTO-REMOVED #include <core/scoring/trie/RotamerTrieBase.hh>
#include <core/scoring/EnergyMap.fwd.hh>
// AUTO-REMOVED #include <core/conformation/symmetry/SymmetricConformation.hh>
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>

// Utility headers
#include <utility/pointer/owning_ptr.hh>
// AUTO-REMOVED #include <utility/vector1.hh>

//Auto Headers
#include <core/conformation/symmetry/SymmetricConformation.fwd.hh>
#include <core/kinematics/Jump.hh>


namespace core {
namespace pack {
namespace rotamer_set {
namespace symmetry {

/// @brief Container for a set of rotamers for use in packing.
/// Rotamers are sorted into groups of the same residue type.
/// Offsets into these rotamer groups are maintained by this class, as is
/// information concerning the "original rotamer" -- the rotamer
/// present on the input pose before packing began.
/// symmetrical version of RotamerSet_
class SymmetricRotamerSet_ : public RotamerSet_
{
public:
	typedef conformation::symmetry::SymmetricConformation SymmetricConformation;
	typedef conformation::symmetry::SymmetryInfoCOP SymmetryInfoCOP;

public:
	SymmetricRotamerSet_();
	virtual ~SymmetricRotamerSet_();

	/// @brief Computes the packers "one body energies" for the set of rotamers.
	virtual
	void
	compute_one_body_energies(
		pose::Pose const & pose,
		scoring::ScoreFunction const & scorefxn,
		task::PackerTask const & task,
		graph::GraphCOP packer_neighbor_graph,
		utility::vector1< core::PackerEnergy > & energies ) const;

	void
	PackerEnergyMultiply(
		utility::vector1< core::PackerEnergy > & energies,
		core::Size factor ) const;

	void
	PackerEnergyAdd(
		utility::vector1< core::PackerEnergy > & energies,
		utility::vector1< core::PackerEnergy > const & add ) const;

	void
	PackerEnergySubtract(
		utility::vector1< core::PackerEnergy > & energies,
		utility::vector1< core::PackerEnergy > const & subtract ) const;

	RotamerSetOP
	orient_rotamer_set_to_symmetric_partner(
		pose::Pose const & pose,
		conformation::ResidueOP residue_in,
		int const & sympos,
		RotamerSetOP rotset_in
	) const;

};

} // namespace symmetry
} // namespace rotamer_set
} // namespace pack
} // namespace core


#endif // INCLUDED_core_pack_RotamerSet_RotamerSet__HH

