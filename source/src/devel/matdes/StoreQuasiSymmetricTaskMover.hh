// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet;
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file devel/matdes/StoreQuasiSymmetricTaskMover.hh
/// @brief Headers for StoreQuasiSymmetricTaskMover class
/// @author Neil King (neilking@uw.edu)

#ifndef INCLUDED_devel_matdes_StoreQuasiSymmetricTaskMover_hh
#define INCLUDED_devel_matdes_StoreQuasiSymmetricTaskMover_hh

//unit headers
#include <devel/matdes/StoreQuasiSymmetricTaskMover.fwd.hh>
#include <protocols/moves/Mover.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <core/pack/task/TaskFactory.hh>

namespace devel {
namespace matdes {

/// @brief mover that prepares a quasisymmetric task based upon a combination
/// of input tasks.
class StoreQuasiSymmetricTaskMover : public protocols::moves::Mover {

public:

	StoreQuasiSymmetricTaskMover();
	~StoreQuasiSymmetricTaskMover();

	virtual void apply( core::pose::Pose & pose  );
	virtual std::string get_name() const;
	protocols::moves::MoverOP clone() const;
	protocols::moves::MoverOP fresh_instance() const;

	void
	parse_my_tag(
		TagCOP tag,
		basic::datacache::DataMap & data_map,
		protocols::filters::Filters_map const &,
		protocols::moves::Movers_map const &,
		core::pose::Pose const &
	);

  // setters
  void quasi_symm_comp( std::string const quasi_symm_comp );
  void num_quasi_repeats( core::Size const num_quasi_repeats );

  // getters
  char quasi_symm_comp() const;
  core::Size num_quasi_repeats() const;

private:
	core::pack::task::TaskFactoryOP task_factory_;
	std::string task_name_;
	bool overwrite_;
	std::string quasi_symm_comp_;
	core::Size num_quasi_repeats_;
};


} // matdes
} // devel

#endif

