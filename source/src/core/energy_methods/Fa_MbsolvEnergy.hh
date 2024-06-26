// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/Fa_MbsolvEnergy.hh
/// @brief  LK-type Membrane Solvation
/// @author Patrick Barth

#ifndef INCLUDED_core_energy_methods_Fa_MbsolvEnergy_hh
#define INCLUDED_core_energy_methods_Fa_MbsolvEnergy_hh

// Unit Headers
#include <core/energy_methods/Fa_MbsolvEnergy.fwd.hh>
#include <core/scoring/MembraneTopology.fwd.hh>
// Package headers
#include <core/conformation/Atom.fwd.hh>
#include <core/scoring/methods/ContextDependentTwoBodyEnergy.hh>
#include <core/scoring/etable/Etable.fwd.hh>
#include <core/scoring/memb_etable/MembEtable.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/scoring/Membrane_FAPotential.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

// Utility headers
#include <utility/vector1.hh>

#include <ObjexxFCL/FArray3D.fwd.hh>

namespace core {
namespace energy_methods {



class Fa_MbsolvEnergy : public core::scoring::methods::ContextDependentTwoBodyEnergy {
public:
	typedef ContextDependentTwoBodyEnergy  parent;
public:

	Fa_MbsolvEnergy( core::scoring::etable::Etable const & etable_in, core::scoring::etable::MembEtable const & memb_etable_in, bool const analytic_membetable_evaluation );


	/// clone
	core::scoring::methods::EnergyMethodOP
	clone() const override;

	void
	setup_for_derivatives(
		pose::Pose & pose,
		core::scoring::ScoreFunction const & scfxn
	) const override;

	/// called during gradient-based minimization inside dfunc
	/**
	F1 and F2 are not zeroed -- contributions from this atom are
	just summed in
	**/
	void
	eval_atom_derivative(
		id::AtomID const & id,
		pose::Pose const & pose,
		kinematics::DomainMap const & domain_map,
		core::scoring::ScoreFunction const & sfxn,
		core::scoring::EnergyMap const & weights,
		Vector & F1,
		Vector & F2
	) const override;


	void
	residue_pair_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap & emap
	) const override;

	bool
	defines_intrares_energy( core::scoring::EnergyMap const & /*weights*/ ) const override { return false; }

	void
	eval_intrares_energy(
		conformation::Residue const &,
		pose::Pose const &,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap & emap
	) const override;

	Distance
	atomic_interaction_cutoff() const override;


	void indicate_required_context_graphs( utility::vector1< bool > & context_graphs_required ) const override;

	core::scoring::MembraneTopology const & MembraneTopology_from_pose( pose::Pose const & pose ) const;
	core::scoring::Membrane_FAEmbed const & Membrane_FAEmbed_from_pose( pose::Pose const & pose ) const;

	void
	setup_for_scoring(
		pose::Pose & pose, core::scoring::ScoreFunction const &
	) const override;

	void
	finalize_total_energy(
		pose::Pose & pose,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap & emap // totals
	) const override;


private:


	void
	get_residue_pair_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		Real & fa_mbsolv_score ) const;

	Real
	eval_lk(
		conformation::Atom const & atom1,
		conformation::Atom const & atom2,
		Real const & d2,
		Real & deriv,
		Real const & f1,
		Real const & f2,
		bool & debug ) const;

	Real
	eval_dE_dR_over_r(
		conformation::Atom const & atom1,
		conformation::Atom const & atom2,
		core::scoring::EnergyMap const & weights,
		Vector & F1,
		Vector & F2,
		Real const & f1,
		Real const & f2
	) const;

	void
	solvationE(
		conformation::Atom const & atom1,
		conformation::Atom const & atom2,
		core::Real dis2,
		core::Real & solvE1,
		core::Real & solvE2,
		core::Real & membsolvE1,
		core::Real & membsolvE2
	) const;

	core::Real
	solv(
		int atom1type,
		int atom2type,
		core::Real dis
	) const;

	core::Real
	solv_piece(
		int atom1type,
		core::Real d
	) const;

	//solvation partial derivative wrt distance from atom i and j
	void
	dsolvationE(
		conformation::Atom const & atom1,
		conformation::Atom const & atom2,
		core::Real dis2,
		core::Real & dsolvE1,
		core::Real & dsolvE2,
		core::Real & dmembsolvE1,
		core::Real & dmembsolvE2
	) const;

	core::Real
	solv_deriv(
		conformation::Atom const & atom,
		core::Real dis
	) const;

	// void
	// distribute_pseudo_base_atom_derivatives( pose::Pose const & pose ) const;

	/////////////////////////////////////////////////////////////////////////////
	// data
	/////////////////////////////////////////////////////////////////////////////

private:
	core::scoring::etable::Etable const & etable_; // shouldn't this be a pointer? Reference count information is (dangerously) lost when
	//a reference is taken, instead of a smart pointer.  There's the potential for a dangling reference with this.
	core::scoring::etable::MembEtable const & memb_etable_;

	/// these guys are taken from the etable
	ObjexxFCL::FArray3D< Real > const & solv1_;
	ObjexxFCL::FArray3D< Real > const & solv2_;
	ObjexxFCL::FArray3D< Real > const & dsolv1_;
	ObjexxFCL::FArray3D< Real > const & dsolv2_;
	ObjexxFCL::FArray3D< Real > const & dsolv_;

	//pba taken from the membrane etable
	ObjexxFCL::FArray3D< Real > const & memb_solv1_;
	ObjexxFCL::FArray3D< Real > const & memb_solv2_;
	ObjexxFCL::FArray3D< Real > const & memb_dsolv1_;
	ObjexxFCL::FArray3D< Real > const & memb_dsolv2_;
	//ObjexxFCL::FArray3D< Real > const & memb_dsolv_;

	utility::vector1< Real > const & lk_dgfree_;
	utility::vector1< Real > const & memb_lk_dgfree_;
	utility::vector1< Real > const & lj_radius_;
	utility::vector1< Real > const & lk_volume_;
	utility::vector1< Real > const & lk_lambda_;


	Real const safe_max_dis2_;
	Real const get_bins_per_A2_;

	bool const verbose_;

	Real max_dis_;
	Real max_normal_dis_;

	core::scoring::Membrane_FAPotential const & potential_;
	bool const analytic_etable_evaluation_;

	/// Used soley when calculating derivatives
	/// Could/should be moved into the Pose's cachable data.
	mutable Real fa_mbsolv_weight_; // hold this while calculating derivatives.
	core::Size version() const override;
};

}
}

#endif // INCLUDED_core_energy_methods_Fa_MbsolvEnergy_HH
