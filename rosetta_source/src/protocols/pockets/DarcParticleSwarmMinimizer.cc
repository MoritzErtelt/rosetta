// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/pockets/DarcParticleSwarmMinimizer.cc
/// @brief
/// @author Karen R. Khar


#include <protocols/pockets/DarcParticleSwarmMinimizer.hh>
#include <core/optimization/ParticleSwarmMinimizer.hh>
#include <utility/exit.hh>
#include <numeric/random/random.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/OptionKeys.hh>
#include <basic/options/keys/fingerprint.OptionKeys.gen.hh>
#include <basic/database/open.hh>
#include <core/pose/Pose.hh>
#include <core/id/AtomID_Map.hh>
#include <core/conformation/Residue.hh>
#include <core/conformation/Conformation.hh>
#include <core/chemical/AtomType.hh>
#include <ObjexxFCL/format.hh>
#include <algorithm>
#include <utility/vector1.hh>

static numeric::random::RandomGenerator my_RG(6172108); // <- Magic number, do not change it!!!

namespace protocols {
namespace pockets {

using namespace ObjexxFCL::fmt;

// note: f_fitness is not used, rather we hard-code the objective function here to enable parellization across particles (for GPU)
//void DarcParticleSwarmMinimizer::score_all_particles(core::optimization::Multifunc & f_fitness, core::optimization::ParticleOPs & particles) {
void DarcParticleSwarmMinimizer::score_all_particles(core::optimization::Multifunc & , core::optimization::ParticleOPs & particles) {

  using namespace core::optimization;

  Size const N = particles.size();
  ligand_natoms_ = pfp_.compute_ligand_natoms();

  for (Size j = 1; j <= N; ++j) {
    core::optimization::Multivec const vars = particles[j]->p_;
    numeric::xyzVector<core::Real> origin_offset;
    origin_offset.x() = vars[1];
    origin_offset.y() = vars[2];
    origin_offset.z() = vars[3];
    core::conformation::ResidueCOP ligand_rsd = pfp_.move_ligand_( nfp_, origin_offset, vars[4], vars[5], vars[6] );
    fill_atom_arrays_(j-1, ligand_rsd );
  }

#ifdef USEOPENCL

  if(nfp_.gpu_.use()){

    int actual_atom_array_size = ligand_natoms_ * N;
    nfp_.gpu_.WriteData(nfp_.gpu_atoms_, nfp_.atom_, sizeof(*nfp_.atom_) * actual_atom_array_size);

    if(!nfp_.gpu_.ExecuteKernel("Check_for_intersection", nfp_.gpu_num_rays_, 256, 64, NULL)) {
      std::cout << "Failed to launch kernel: " << nfp_.gpu_.lastErrorStr() << std::endl;
      exit(1);
    }

    if(!nfp_.gpu_.ExecuteKernel("Get_scores", N, 256, 64, NULL)) {
      std::cout << "Failed to launch kernel: " << nfp_.gpu_.lastErrorStr() << std::endl;
      exit(1);
    }

    nfp_.gpu_.ReadData(nfp_.particle_scores_, nfp_.gpu_particle_scores_, sizeof(*nfp_.particle_scores_) * N);

    for (core::Size j = 0; j < N; ++j) {
      core::Real particle_score = nfp_.particle_scores_[j];
      particles[j+1]->set_score(particle_score);
    }

  } else {

#endif

    for (core::Size j = 1; j <= N; ++j) {
      core::Real particle_score = DarcPSO_fp_compare_( j-1, missing_pt_, steric_, extra_pt_ );
      particles[j]->set_score(particle_score);
    }

#ifdef USEOPENCL
  }
#endif

}

void DarcParticleSwarmMinimizer::fill_atom_arrays_( core::Size particle_inx, core::conformation::ResidueCOP ligand_rsd ) {

  using namespace basic::options;
  core::Real const radius_scale = option[ OptionKeys::fingerprint::atom_radius_scale ];
  core::Real const atom_buffer = option[ OptionKeys::fingerprint::atom_radius_buffer ];

  for (Size i = 1; i <= ligand_natoms_; ++i) {

    core::Size const curr_array_inx = ( ligand_natoms_ * particle_inx ) + ( i-1 );

    numeric::xyzVector<core::Real> this_atomcoors = ligand_rsd->atom(i).xyz() - pfp_.origin();
    core::Real const this_atom_radius = ( ligand_rsd->atom_type(i).lj_radius() - atom_buffer ) * radius_scale;
    nfp_.atom_[curr_array_inx].x = this_atomcoors.x();
    nfp_.atom_[curr_array_inx].y = this_atomcoors.y();
    nfp_.atom_[curr_array_inx].z = this_atomcoors.z();
    nfp_.atom_[curr_array_inx].w = this_atom_radius;

    // find the atom center (in spherical coors)
    spherical_coor_triplet atom_center;
    convert_cartesian_to_spherical_coor_triplet( this_atomcoors, atom_center );

    core::Real const tmp_atomx=this_atomcoors.x();
    core::Real const tmp_atomy=this_atomcoors.y();
    core::Real const tmp_atomz=this_atomcoors.z();

    core::Real curr_max_phi( 999 );
    core::Real curr_min_phi( -999 );
    if ( std::abs(tmp_atomz) > 0.00001 ) {
      core::Real const inside_phi_asin = this_atom_radius / tmp_atomz;
      if ( std::abs(inside_phi_asin) < 1. ) {
	core::Real const max_angular_displacement_phi = std::abs( asin( inside_phi_asin ) );
	curr_max_phi = atom_center.phi + max_angular_displacement_phi;
	curr_min_phi = atom_center.phi - max_angular_displacement_phi;
      }
    }

    core::Real curr_max_psi( 999 );
    core::Real curr_min_psi( -999 );
    if ( ( std::abs(tmp_atomx) > 0.00001 ) || ( std::abs(tmp_atomx) > 0.00001 ) ) {
      core::Real const inside_psi_asin = this_atom_radius / sqrt( (tmp_atomx*tmp_atomx) + (tmp_atomy*tmp_atomy) );
      if ( std::abs(inside_psi_asin) < 1. ) {
	core::Real const max_angular_displacement_psi = std::abs( asin( inside_psi_asin ) );
	curr_max_psi = atom_center.psi + max_angular_displacement_psi;
	curr_min_psi = atom_center.psi - max_angular_displacement_psi;
      }
    }

    if ( curr_max_phi > nfp_.ligand_maxmin_phipsi_[particle_inx].x ) nfp_.ligand_maxmin_phipsi_[particle_inx].x = curr_max_phi;
    if ( curr_max_psi > nfp_.ligand_maxmin_phipsi_[particle_inx].y ) nfp_.ligand_maxmin_phipsi_[particle_inx].y = curr_max_psi;
    if ( curr_min_phi < nfp_.ligand_maxmin_phipsi_[particle_inx].z ) nfp_.ligand_maxmin_phipsi_[particle_inx].z = curr_min_phi;
    if ( curr_min_psi < nfp_.ligand_maxmin_phipsi_[particle_inx].w ) nfp_.ligand_maxmin_phipsi_[particle_inx].w = curr_min_psi;
    nfp_.atom_maxmin_phipsi_[curr_array_inx].x = curr_max_phi;
    nfp_.atom_maxmin_phipsi_[curr_array_inx].y = curr_max_psi;
    nfp_.atom_maxmin_phipsi_[curr_array_inx].z = curr_min_phi;
    nfp_.atom_maxmin_phipsi_[curr_array_inx].w = curr_min_psi;

  }

}

core::Real DarcParticleSwarmMinimizer::DarcPSO_fp_compare_( core::Size particle_inx, core::Real const & missing_point_weight, core::Real const & steric_weight, core::Real const & extra_point_weight ) {

  core::Real Total_score = 0;
  core::Size num_rays = 0;

  for (std::list<spherical_coor_triplet>::const_iterator ni = nfp_.triplet_fingerprint_data().begin(); ni != nfp_.triplet_fingerprint_data().end(); ++ni) {

    core::Real curr_phi = ni->phi;
    core::Real curr_psi = ni->psi;
    core::Real best_rho_sq(9999.);
    core::Size best_intersecting_atom(0);
    for (Size i = 1, i_end = ligand_natoms_; i <= i_end; ++i) {

      core::Size const curr_array_inx = ( ligand_natoms_ * particle_inx ) + ( i-1 );

      if ( nfp_.atom_[curr_array_inx].w < 0.001 ) continue;

      while ( curr_phi < nfp_.atom_maxmin_phipsi_[curr_array_inx].z ) {
	curr_phi += numeric::constants::r::pi_2;
      }
      while ( curr_phi > nfp_.atom_maxmin_phipsi_[curr_array_inx].x ) {
	curr_phi -= numeric::constants::r::pi_2;
      }
      while ( curr_psi < nfp_.atom_maxmin_phipsi_[curr_array_inx].w ) {
	curr_psi += numeric::constants::r::pi_2;
      }
      while ( curr_psi > nfp_.atom_maxmin_phipsi_[curr_array_inx].y ) {
	curr_psi -= numeric::constants::r::pi_2;
      }
      if ( curr_phi < nfp_.atom_maxmin_phipsi_[curr_array_inx].z ) continue;
      if ( curr_psi < nfp_.atom_maxmin_phipsi_[curr_array_inx].w ) continue;
      if ( curr_phi > nfp_.atom_maxmin_phipsi_[curr_array_inx].x ) continue;
      if ( curr_psi > nfp_.atom_maxmin_phipsi_[curr_array_inx].y ) continue;

      core::Real const min_intersect_SQ = Find_Closest_Intersect_SQ(curr_phi,curr_psi,nfp_.atom_[curr_array_inx].x,nfp_.atom_[curr_array_inx].y,nfp_.atom_[curr_array_inx].z,nfp_.atom_[curr_array_inx].w);

      if ( min_intersect_SQ < best_rho_sq ) {
	best_rho_sq = min_intersect_SQ;
      }
    }

    core::Real plaid_rho = 9999.;
    if ( best_rho_sq < 9998. ) {
      plaid_rho = sqrt(best_rho_sq);
    }

    if ( (plaid_rho > 9998.) && (ni->rho > 0.001) ) {
      Total_score += missing_point_weight;
      num_rays++;
    }
    if ( (plaid_rho < 9999.) && (ni->rho < 0.001 ) ) {
      Total_score += extra_point_weight;
      num_rays++;
    }
    if ( (plaid_rho < 9999.) && (ni->rho > 0.001 ) ) {
      core::Real dist_deviation = ( plaid_rho - ni->rho );
      if (dist_deviation < 0.0) dist_deviation = ( ni->rho - plaid_rho ) * steric_weight;
      Total_score += dist_deviation;
      num_rays++;
    }
  }

  return (Total_score/num_rays);
}

} // namespace pockets
} // namespace protocols
