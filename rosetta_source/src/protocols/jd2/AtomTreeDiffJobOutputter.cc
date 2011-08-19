// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/jd2/AtomTreeDiffJobOutputter.cc
/// @brief  August 2008 job distributor as planned at RosettaCon08 - AtomTreeDiffJobOutputter
/// @author Steven Lewis smlewi@gmail.com

///Unit headers
#include <protocols/jd2/AtomTreeDiffJobOutputter.hh>
#include <protocols/jd2/Job.hh>
#include <protocols/jd2/Job.fwd.hh>

///Project headers
#include <core/pose/Pose.hh>
#include <core/import_pose/atom_tree_diffs/atom_tree_diff.hh>

///Utility headers
#include <basic/Tracer.hh>
#include <core/svn_version.hh>
#include <basic/options/option.hh>
#include <utility/io/ozstream.hh>
// AUTO-REMOVED #include <utility/io/izstream.hh>
#include <utility/file/file_sys_util.hh>

// option key includes

#include <basic/options/keys/out.OptionKeys.gen.hh>
#include <basic/options/keys/run.OptionKeys.gen.hh>

///C++ headers
//#include <string>
//#include <sstream>

static basic::Tracer TR("protocols.jd2.AtomTreeDiffJobOutputter");

namespace protocols {
namespace jd2 {

using namespace basic::options;
using namespace basic::options::OptionKeys;

AtomTreeDiffJobOutputter::AtomTreeDiffJobOutputter(){

	// Add directory path and prefix/suffix (if specified) to plain file name:
	outfile_name_= basic::options::option[ OptionKeys::out::file::atom_tree_diff ]();
	{
		/// TODO abstract this section to a generic file for all job outputters
		utility::file::FileName outfile(outfile_name_);
		std::ostringstream oss;
		oss << basic::options::option[ basic::options::OptionKeys::out::prefix ]() << outfile.base()
		    << basic::options::option[ basic::options::OptionKeys::out::suffix ]();
		outfile.base( oss.str() );
		outfile.path( basic::options::option[ basic::options::OptionKeys::out::path::pdb ]().path() );
		outfile.vol( basic::options::option[ basic::options::OptionKeys::out::path::pdb ]().vol() );
		if( basic::options::option[ basic::options::OptionKeys::out::pdb_gz ] && outfile.ext() != "gz" ) {
			outfile.ext( ".gz" ); // else use input extension
		}
		outfile_name_ = outfile.name();
	}

	//Looks like we want to only have the file open when we're actually writing, moved this to dump_pose

	/*
	if( utility::file::file_exists(outfile_name) ) {
		atom_tree_diff_.read_file(outfile_name);
		out_.open_append( outfile_name.c_str() );
	} else {
		out_.open( outfile_name.c_str() );
		if( basic::options::option[ basic::options::OptionKeys::run::version ] ) {
			out_ << "# Mini-Rosetta version " << core::minirosetta_svn_version() << " from " << core::minirosetta_svn_url() << "\n";
		}
	}
	if ( !out_.good() ) {
		utility_exit_with_message( "Unable to open file: " + outfile_name + "\n" );
	}
	*/
}

AtomTreeDiffJobOutputter::~AtomTreeDiffJobOutputter(){}

void
AtomTreeDiffJobOutputter::final_pose( JobCOP job, core::pose::Pose const & pose ){
	Job::StringRealPairs::const_iterator begin= job->output_string_real_pairs_begin();
	Job::StringRealPairs::const_iterator end= job->output_string_real_pairs_end();

	std::map< std::string, core::Real > scores(begin, end);
	std::string output= output_name(job);
	dump_pose( output, pose, scores);

}

///@brief Appends pose to atom tree diff file
void
AtomTreeDiffJobOutputter::dump_pose(
	std::string const & tag,
	core::pose::Pose const & pose,
	std::map< std::string, core::Real > scores
){

	if( utility::file::file_exists(outfile_name_) ) {
		//atom_tree_diff_.read_file(outfile_name_);
		out_.open_append( outfile_name_.c_str() );
	} else {
		out_.open( outfile_name_.c_str() );
		if( basic::options::option[ basic::options::OptionKeys::run::version ] ) {
			out_ << "# Mini-Rosetta version " << core::minirosetta_svn_version() << " from " << core::minirosetta_svn_url() << "\n";
		}
	}
	if ( !out_.good() ) {
		utility_exit_with_message( "Unable to open file: " + outfile_name_ + "\n" );
	}

	core::Size end= tag.find_last_of('_');
	std::string const ref_tag= tag.substr(0, end);
	TR<< "ref_tag: "<< ref_tag<< std::endl;
	if( last_ref_tag_ != ref_tag ){
		TR<< "writing reference pose"<< std::endl;
		std::map< std::string, core::Real > temp_scores;
		temp_scores["is_reference_pose"]= 1;
		core::import_pose::atom_tree_diffs::dump_reference_pose(out_, "%REF%_"+tag, temp_scores, pose);
		last_ref_tag_= ref_tag;// deep copy for reference pose
		last_ref_pose_= pose;// deep copy for reference pose
	}
	if( used_tags_.find(tag) != used_tags_.end() ){
		basic::Error() << "Tag " << tag << " already exists in silent file; writing structure anyway..." << std::endl;
	}
	core::import_pose::atom_tree_diffs::dump_atom_tree_diff(out_, tag, scores, last_ref_pose_, pose);
	used_tags_.insert(tag);
	// Can't flush compressed streams -- results in file truncation
	if ( out_.uncompressed() ) out_.flush();
	out_.close();
}


void
AtomTreeDiffJobOutputter::other_pose( JobCOP /*job*/, core::pose::Pose const & /*pose*/, std::string const & /*tag*/ ){
	// do something with this function later
	return;
}

bool
AtomTreeDiffJobOutputter::job_has_completed( JobCOP job ){
	// was the job completed beforehand ( in the silent file before this app even started) ?
	if( used_tags_.find( output_name( job )) != used_tags_.end() ) return true;
	// did WE complete the job later ?
	if( job->completed() ) return true;

	return false;
}

std::string
AtomTreeDiffJobOutputter::output_name( JobCOP job ){
	return affixed_numbered_name( job );
}

}//jd2
}//protocols
