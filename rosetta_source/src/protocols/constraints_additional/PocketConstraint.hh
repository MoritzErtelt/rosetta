// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is made available under the Rosetta Commons license.
// See http://www.rosettacommons.org/license
// (C) 199x-2007 University of Washington
// (C) 199x-2007 University of California Santa Cruz
// (C) 199x-2007 University of California San Francisco
// (C) 199x-2007 Johns Hopkins University
// (C) 199x-2007 University of North Carolina, Chapel Hill
// (C) 199x-2007 Vanderbilt University

/// @file   protocols/pockets/constraints/PocketConstraint.hh
///
/// @brief
/// @author David Johnson


#ifndef INCLUDED_protocols_pockets_PocketConstraint_hh
#define INCLUDED_protocols_pockets_PocketConstraint_hh

#include <core/scoring/constraints/Constraint.hh>
#include <protocols/constraints_additional/PocketConstraint.fwd.hh>
#include <protocols/pockets/PocketGrid.hh>

#include <math.h>
#include <core/id/AtomID.hh>
#include <core/pose/Pose.fwd.hh>
#include <utility/vector1.hh>
#include <utility/pointer/owning_ptr.hh>
#include <utility/pointer/ReferenceCount.hh>
#include <list>

namespace protocols {
namespace constraints_additional {

///@brief This constraint favors creating a pocket suitable for a small-molecule
///
class PocketConstraint : public core::scoring::constraints::Constraint
{
public:
  virtual std::string type() const {
    return "Pocket";
  }


	PocketConstraint( );
  PocketConstraint( core::pose::Pose const & pose );
  PocketConstraint( const PocketConstraint& old );

  void init(core::pose::Pose const & pose);

  core::Size target_res() const { return seqpos_; }
  virtual ~PocketConstraint();

  virtual core::Size natoms() const { return atom_ids_.size(); };

  virtual core::id::AtomID const & atom( core::Size const index ) const { return atom_ids_[index]; };

  void show_def( std::ostream& out, core::pose::Pose const & pose ) const;
  void read_def( std::istream& in, core::pose::Pose const & pose, core::scoring::constraints::FuncFactory const & func_factory );

  virtual
  void score( core::scoring::constraints::XYZ_Func const & xyz_func, core::scoring::EnergyMap const & weights, core::scoring::EnergyMap & emap ) const;

  virtual
  void
  fill_f1_f2(
    core::id::AtomID const & ,
    core::scoring::constraints::XYZ_Func const & ,
    core::Vector & ,
    core::Vector & ,
    core::scoring::EnergyMap const & weights
  ) const;

  virtual
  core::scoring::constraints::ConstraintOP clone() const;

  void set_target_res( core::pose::Pose const & pose, core::Size new_seqpos );
  void set_target_res_pdb(core::pose::Pose const & pose, std::string resid );

private:

  core::Size seqpos_;
  core::Size totalres_;
  core::Size angles_;
  core::Real weight_;
  mutable protocols::pockets::PocketGridOP pocketgrid_;
  utility::vector1< AtomID > atom_ids_;
  bool dumppdb_;
  std::vector< core::conformation::ResidueOP > residues_;

}; // PocketConstraint

typedef utility::pointer::owning_ptr< PocketConstraint > PocketConstraintOP;

} // namespace constraints_additional
} // namespace protocols


#endif // INCLUDED_protocols_pockets_PocketConstraint_HH
