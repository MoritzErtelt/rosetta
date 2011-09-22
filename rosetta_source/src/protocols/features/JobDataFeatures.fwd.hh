// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   /git/src/protocols/features/JobDataFeatures.fwd.hh
/// @author Sam DeLuca

#ifndef INCLUDED_protocols_features_JobDataFeatures_fwd_hh_
#define INCLUDED_protocols_features_JobDataFeatures_fwd_hh_

#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace features {

class JobDataFeatures;
typedef utility::pointer::owning_ptr<JobDataFeatures> JobDataFeaturesOP;
typedef utility::pointer::owning_ptr<JobDataFeatures const> JobDataFeaturesCOP;

}
}


#endif /* JOBDATAFEATURES_FWD_HH_ */
