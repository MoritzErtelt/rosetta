// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/jd2/EnzdesJobInputter.cc
/// @brief  job inputter that adds some stuff for enzdes to the pose
/// @author Florian Richter ( floric@u.washington.edu), Sagar Khare (khares@u.washington.edu)

///Unit headers
#include <protocols/jd2/EnzdesJobInputter.hh>

//package headers
#include <protocols/jd2/Job.hh>
#include <protocols/enzdes/enzdes_util.hh>
#include <protocols/enzdes/EnzdesCacheableObserver.hh>
#include <protocols/enzdes/EnzdesLoopsFile.hh>

///Project headers
#include <basic/options/option.hh>
#include <core/pose/Pose.hh>
#include <core/pose/datacache/CacheableObserverType.hh>
#include <core/pose/datacache/ObserverCache.hh>
#include <core/pose/datacache/cacheable_observers.hh>

#ifdef WIN32
// required for VS2005 build
#include <core/conformation/Residue.hh>
#endif

///Utility headers
#include <basic/Tracer.hh>

///C++ headers
#include <string>

// option key includes
#include <basic/options/keys/enzdes.OptionKeys.gen.hh>

//Auto Headers
#include <utility/options/keys/BooleanOptionKey.hh>


static basic::Tracer TR("protocols.jd2.EnzdesJobInputter");

namespace protocols {
namespace jd2 {

EnzdesJobInputter::EnzdesJobInputter()
	: enz_loops_file_(NULL)
{
	TR << "Instantiate EnzdesJobInputter" << std::endl;
	if( basic::options::option[ basic::options::OptionKeys::enzdes::enz_loops_file ].user() ){

		enz_loops_file_ = new protocols::enzdes::EnzdesLoopsFile();
		if( !enz_loops_file_->read_loops_file( basic::options::option[ basic::options::OptionKeys::enzdes::enz_loops_file ] ) ){
			utility_exit_with_message("Reading enzdes loops file failed");
		}
	}
}

EnzdesJobInputter::~EnzdesJobInputter(){
}

void
EnzdesJobInputter::pose_from_job( core::pose::Pose & pose, JobOP job){
	TR << "EnzdesJobInputter::pose_from_job" << std::endl;

	std::string input_tag( job->input_tag() );
	protocols::enzdes::enzutil::read_pose_from_pdb( pose, input_tag );
	if( enz_loops_file_){
		protocols::enzdes::get_enzdes_observer( pose )->set_enzdes_loops_file( enz_loops_file_ );
		core::pose::datacache::SpecialSegmentsObserverOP segob = new core::pose::datacache::SpecialSegmentsObserver();
		segob->clear();
		for( core::Size i =1; i <= enz_loops_file_->num_loops(); ++i){
			segob->add_segment(enz_loops_file_->loop_info( i )->start(), enz_loops_file_->loop_info( i )->stop() + 1 /*segment convention*/ );
		}
		pose.observer_cache().set(  core::pose::datacache::CacheableObserverType::SPECIAL_SEGMENTS_OBSERVER, segob, true );
	}

	load_pose_into_job(pose, job);
}

}//jd2
}//protocols
