// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/nonlocal/NonlocalAbinitio.hh
/// @author Christopher Miles (cmiles@uw.edu)

#ifndef PROTOCOLS_NONLOCAL_NONLOCALABINITIO_HH_
#define PROTOCOLS_NONLOCAL_NONLOCALABINITIO_HH_

// Unit header
#include <protocols/nonlocal/NonlocalAbinitio.fwd.hh>

// C/C++ headers
#include <string>

// Utility headers
#include <utility/vector1.hh>

// Project headers
#include <core/types.hh>
#include <core/fragment/FragSet.hh>
#include <core/fragment/SecondaryStructure.hh>
#include <core/pose/Pose.fwd.hh>
#include <protocols/moves/Mover.hh>

// Package headers
#include <protocols/nonlocal/NLGrouping.hh>
#include <protocols/nonlocal/TreeBuilder.hh>

namespace protocols {
namespace nonlocal {

class NonlocalAbinitio : public protocols::moves::Mover {
  typedef protocols::moves::Mover Parent;
  typedef utility::vector1<NLGrouping> NonlocalGroupings;

public:
  /// @brief Enumeration type representing the high-level kinematic modes.
  enum KinematicPolicy { RIGID, SEMI_RIGID };

  /// @brief Enumeration type representing the conformational space search
  /// strategies. In refinement mode, conformational search does not wander far
  /// from the starting structure. This is useful for inputs where only a small
  /// number of residues need to be modified. Conversely, in exploration mode
  /// conformational search is allowed to wander away from the starting point,
  /// guided by the kinematics of the system and (optionally) any user-specified
  /// restraints.
  enum SearchStrategy { REFINE, EXPLORE };

  /// @brief Constructs a new mover with the specified mode and search strategy
  /// by reading non-local pairings information from the filename specified by
  /// -nonlocal:moves.
  NonlocalAbinitio(KinematicPolicy policy = RIGID,
                   SearchStrategy strategy = REFINE);

  /// @brief Constructs a new mover with the specified mode, search strategy,
  /// and non-local pairings.
  NonlocalAbinitio(const NonlocalGroupings& groupings,
                   KinematicPolicy policy = RIGID,
                   SearchStrategy strategy = REFINE);

  /// @brief Chooses a non-local grouping at random as the starting point for
  /// the simulation, then proceeds with kinematic ab initio.
  void apply(core::pose::Pose& pose);

  /// @brief Returns the name of this mover.
  std::string get_name() const;

  /// @brief Creates a copy of this instance
  protocols::moves::MoverOP clone() const;

  /// @brief Creates a new instance by calling the no-argument constructor
  protocols::moves::MoverOP fresh_instance() const;

  // -- Accessors -- //

  /// @brief Returns the set of non-local pairings
  const NonlocalGroupings& groupings() const;

  /// @brief Returns the kinematic policy in effect
  KinematicPolicy kinematic_policy() const;

  /// @brief Returns the search strategy in effect
  SearchStrategy search_strategy() const;

  /// @brief Returns a pointer to the large fragment library
  core::fragment::FragSetOP fragments_large() const;

  /// @brief Returns a pointer to the small fragment library
  core::fragment::FragSetOP fragments_small() const;

  /// @brief Registers applicable options
  static void register_options();

private:
  /// --- Utility Methods --- ///
  void initialize(const NonlocalGroupings& groupings,
                  KinematicPolicy policy,
                  SearchStrategy strategy);

  /// @brief Determines whether the caller has specified all required options.
  /// If a required option is missing, exits with an error message.
  void check_required_options() const;

  /// @brief Constructs the fold tree
  TreeBuilderOP make_fold_tree(const NLGrouping& grouping, core::pose::Pose* pose) const;

  /// @brief Prepares a MoveMap to enforce restrictions on modifications to the
  /// system's degrees of freedom. If member variable <mode_> is set to RIGID,
  /// backbone torsions are prevented for each entry in <grouping>. In order to
  /// protect the jumps, residues adjacent to cutpoints cannot be modified.
  void prepare_movemap(const NLGrouping& grouping,
                       const core::pose::Pose& pose,
                       core::kinematics::MoveMapOP movable);

  /// @brief Orients and places the rigid chunks in <grouping> into <pose>
  void superimpose(const NLGrouping& grouping, core::pose::Pose* pose) const;

  /// @brief Prepares <pose> for broken-chain folding by applying fragment moves
  /// and loop closure
  void initial_closure(core::pose::Pose* pose) const;

  /// @brief Closes any chainbreaks that exist after broken chain folding
  void final_closure(core::pose::Pose* pose) const;

  /// @brief Relax into constraints
  void relax(core::pose::Pose* pose) const;

  /// --- Members --- ///

  /// @brief Prior or predicted knowledge of the protein structure
  NonlocalGroupings groupings_;

  /// @brief Specifies how to treat residues in the selected NLGrouping
  KinematicPolicy kinematic_policy_;

  /// @brief Specifies how conformational search should proceed
  SearchStrategy search_strategy_;

  /// @brief Large fragment library
  core::fragment::FragSetOP fragments_lg_;

  /// @brief Small fragment library
  core::fragment::FragSetOP fragments_sm_;

  /// @brief Predicted secondary structure
  core::fragment::SecondaryStructureOP secondary_struct_;
};

}  // namespace nonlocal
}  // namespace protocols

#endif  // PROTOCOLS_NONLOCAL_NONLOCALABINITIO_HH_
