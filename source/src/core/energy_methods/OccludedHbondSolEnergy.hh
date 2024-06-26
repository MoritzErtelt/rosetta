// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/geometric_solvation/OccludedHbondSolEnergy.hh
/// @brief  Pairwise-decomposable, continuous solvation model based on penalizing potential for Hbonding to solvent.
///  This is the pwSHO model.
/// @author John Karanicolas
/// @author Andrea Bazzoli


#ifndef INCLUDED_core_scoring_geometric_solvation_OccludedHbondSolEnergy_hh
#define INCLUDED_core_scoring_geometric_solvation_OccludedHbondSolEnergy_hh

#include <core/types.hh>

// Unit Headers
#include <core/energy_methods/OccludedHbondSolEnergy.fwd.hh>

// Package headers
#include <core/scoring/methods/ContextIndependentTwoBodyEnergy.hh>
#include <core/scoring/methods/EnergyMethodOptions.fwd.hh>
#include <core/scoring/geometric_solvation/DatabaseOccSolEne.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

#include <core/chemical/AtomTypeSet.fwd.hh>

#include <utility/vector1.hh>

//#include <core/scoring/EnergyMap.hh>

namespace core {
namespace energy_methods {

extern Vector dummy_deriv_vector_;

class OccludedHbondSolEnergy : public core::scoring::methods::ContextIndependentTwoBodyEnergy  {
public:
	typedef core::scoring::methods::ContextIndependentTwoBodyEnergy  parent;
public:

	OccludedHbondSolEnergy( core::scoring::methods::EnergyMethodOptions const & options, bool const verbose = false );

	OccludedHbondSolEnergy( OccludedHbondSolEnergy const & src );

	core::scoring::methods::EnergyMethodOP
	clone() const override;

	void
	setup_for_scoring( pose::Pose & pose, core::scoring::ScoreFunction const & ) const override;

	void
	setup_for_derivatives( pose::Pose & pose, core::scoring::ScoreFunction const & ) const override;

	void
	residue_pair_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & ,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap & emap
	) const override;

	void
	eval_intrares_energy(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		core::scoring::ScoreFunction const & scorefxn,
		core::scoring::EnergyMap & emap
	) const override;

	/// @brief Inform inquiring algorithms that this energy method will opt-in to the
	/// residue-pair decomposable derivative evaluation scheme.
	bool
	minimize_in_whole_structure_context( pose::Pose const & ) const override { return false; }

	bool
	defines_score_for_residue_pair(
		conformation::Residue const & res1,
		conformation::Residue const & res2,
		bool res_moving_wrt_eachother
	) const override;

	void
	eval_residue_pair_derivatives(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		core::scoring::ResSingleMinimizationData const &,
		core::scoring::ResSingleMinimizationData const &,
		core::scoring::ResPairMinimizationData const & min_data,
		pose::Pose const & pose, // provides context
		core::scoring::EnergyMap const & weights,
		utility::vector1< core::scoring::DerivVectorPair > & r1_atom_derivs,
		utility::vector1< core::scoring::DerivVectorPair > & r2_atom_derivs
	) const override;

	void
	eval_intrares_derivatives(
		conformation::Residue const & rsd,
		core::scoring::ResSingleMinimizationData const & min_data,
		pose::Pose const & pose,
		core::scoring::EnergyMap const & weights,
		utility::vector1< core::scoring::DerivVectorPair > & atom_derivs
	) const override;

	/*void
	deprecated_eval_atom_derivative(
	id::AtomID const & atom_id,
	pose::Pose const & pose,
	kinematics::DomainMap const &,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap const & weights,
	Vector & F1,
	Vector & F2
	) const;*/

	// Note: This could change - see notes in the .cc re double-counting. If it does,
	// eval_atom_derivative has to change too.
	bool
	defines_intrares_energy( core::scoring::EnergyMap const & ) const override { return true; };

	Distance
	atomic_interaction_cutoff() const override;

	void indicate_required_context_graphs( utility::vector1< bool > &  ) const override {};

private:

	/// @brief initializes amplitude scaling factors from command-line
	void init_amp_scaling_factors();

	Real
	res_res_occ_sol_one_way(
		conformation::Residue const & polar_rsd,
		conformation::Residue const & occ_rsd ) const;

	void
	eval_residue_pair_derivatives_one_way(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		core::scoring::EnergyMap const & weights,
		utility::vector1< core::scoring::DerivVectorPair > & r1_atom_derivs,
		utility::vector1< core::scoring::DerivVectorPair > & r2_atom_derivs
	) const;


	void
	get_atom_atom_occ_solvation(
		Size const don_h_atom,
		Size const don_base_atom,
		conformation::Residue const & don_rsd,
		Size const occ_atom,
		conformation::Residue const & occ_rsd,
		Real & energy,
		bool const update_deriv = false,
		Real const occ_sol_fitted_weight = 0.0,
		//bool const update_deriv_base = false,
		//bool const update_deriv_occ = false,
		Vector & f1_base = dummy_deriv_vector_,
		Vector & f2_base = dummy_deriv_vector_,
		Vector & f1_polar = dummy_deriv_vector_,
		Vector & f2_polar = dummy_deriv_vector_,
		Vector & f1_occ = dummy_deriv_vector_,
		Vector & f2_occ = dummy_deriv_vector_
	) const;

	Real
	get_cos_angle(
		Vector const & base_atom_xyz,
		Vector const & polar_atom_xyz,
		Vector const & occluding_atom_xyz ) const;

	bool
	atom_is_donor_h( conformation::Residue const & rsd, Size const atom ) const;

	bool
	atom_is_acceptor( conformation::Residue const & rsd, Size const atom ) const;

	bool
	atom_is_valid_base( conformation::Residue const & rsd, Size const atom ) const;


private:

	// atom type set (assumed to be FA_STANDARD)
	chemical::AtomTypeSetCOP atom_type_set_ptr_;

	// atom-type-specific scaling factors for tha amplitudes of Gaussians (those of non-polar atoms are left at 0)
	utility::vector1<core::Real> amp_scaling_factors_;

	// const-ref to scoring database
	scoring::geometric_solvation::DatabaseOccSolEne const & occ_hbond_sol_database_;

	bool const verbose_;
	core::Size version() const override;

};

} // scoring
} // core

#endif

