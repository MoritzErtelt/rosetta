// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @brief
/// @author ashworth

#ifndef INCLUDED_protocols_dna_WatsonCrickRotamerCouplingsCreator_hh
#define INCLUDED_protocols_dna_WatsonCrickRotamerCouplingsCreator_hh

#include <core/pack/task/operation/TaskOperationCreator.hh>

namespace protocols {
namespace dna {

class WatsonCrickRotamerCouplingsCreator : public core::pack::task::operation::TaskOperationCreator {
public:
	virtual core::pack::task::operation::TaskOperationOP create_task_operation() const;
	virtual std::string keyname() const { return "WatsonCrickRotamerCouplings"; }
};

} // namespace dna
} // namespace protocols

#endif
