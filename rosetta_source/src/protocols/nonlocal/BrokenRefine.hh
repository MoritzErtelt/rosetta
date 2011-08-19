// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/nonlocal/BrokenRefine.hh
/// @author Christopher Miles (cmiles@uw.edu)

#ifndef PROTOCOLS_NONLOCAL_BROKENREFINE_HH_
#define PROTOCOLS_NONLOCAL_BROKENREFINE_HH_

// Unit header
#include <protocols/nonlocal/BrokenRefine.fwd.hh>

// C/C++ headers
#include <string>

// Project headers
#include <core/types.hh>
#include <core/fragment/FragSet.fwd.hh>
#include <core/kinematics/MoveMap.fwd.hh>

// Package headers
#include <protocols/nonlocal/BrokenBase.hh>

namespace protocols {
namespace nonlocal {

/// @class Broken-chain refinement protocol specialized for NonlocalAbinitio.
/// Movers are enqueued in the constructor and executed in order in the base
/// class. If specialization is required, simply override apply().
class BrokenRefine : public BrokenBase {
 public:
  BrokenRefine(core::fragment::FragSetOP fragments_small,
               core::kinematics::MoveMapOP movable);

  /// @brief Returns the name of this mover
  std::string get_name() const;

 private:
  /// @brief Returns the number of cycles for stage i
  core::Size num_cycles(int stage);
};

}  // namespace nonlocal
}  // namespace protocols

#endif  // PROTOCOLS_NONLOCAL_BROKENREFINE_HH_
