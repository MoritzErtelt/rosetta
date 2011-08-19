// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/pack/task/ResfileReader.cc
/// @brief  implementation of resfile reader and its command classes
/// @author Gordon Lemmon (glemmon@gmail.com)

// Unit Headers
#include <protocols/ligand_docking/InterfaceScoreCalculator.hh>
#include <protocols/ligand_docking/InterfaceScoreCalculatorCreator.hh>

#include <utility/exit.hh>
#include <basic/Tracer.hh>
#include <core/types.hh>
#include <utility/tag/Tag.hh>
#include <protocols/moves/DataMap.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>

#include <core/pose/util.hh>
#include <protocols/ligand_docking/ligand_scores.hh>

//Auto Headers
#include <core/pose/Pose.hh>
#include <core/import_pose/import_pose.hh>
#include <protocols/jd2/JobDistributor.hh>

#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/Energies.hh>

using basic::T;
using basic::Error;
using basic::Warning;

namespace protocols {
namespace ligand_docking {

static basic::Tracer InterfaceScoreCalculator_tracer("protocols.ligand_docking.ligand_options.InterfaceScoreCalculator", basic::t_debug);

std::string
InterfaceScoreCalculatorCreator::keyname() const
{
	return InterfaceScoreCalculatorCreator::mover_name();
}

protocols::moves::MoverOP
InterfaceScoreCalculatorCreator::create_mover() const {
	return new InterfaceScoreCalculator;
}

std::string
InterfaceScoreCalculatorCreator::mover_name()
{
	return "InterfaceScoreCalculator";
}

///@brief
InterfaceScoreCalculator::InterfaceScoreCalculator():
		Mover("InterfaceScoreCalculator"),
		chains_(),
		native_(NULL),
		score_fxn_(NULL),
		grid_manager_(NULL){}

InterfaceScoreCalculator::InterfaceScoreCalculator(InterfaceScoreCalculator const & that):
	    //utility::pointer::ReferenceCount(),
		protocols::moves::Mover( that ),
		chains_(that.chains_),
		native_(that.native_),
		score_fxn_(that.score_fxn_),
		grid_manager_(that.grid_manager_)
{}

InterfaceScoreCalculator::~InterfaceScoreCalculator() {}

protocols::moves::MoverOP InterfaceScoreCalculator::clone() const {
	return new InterfaceScoreCalculator( *this );
}

protocols::moves::MoverOP InterfaceScoreCalculator::fresh_instance() const {
	return new InterfaceScoreCalculator;
}

std::string InterfaceScoreCalculator::get_name() const{
	return "InterfaceScoreCalculator";
}

///@brief parse XML (specifically in the context of the parser/scripting scheme)
void
InterfaceScoreCalculator::parse_my_tag(
		utility::tag::TagPtr const tag,
		protocols::moves::DataMap & datamap,
		protocols::filters::Filters_map const & /*filters*/,
		protocols::moves::Movers_map const & /*movers*/,
		core::pose::Pose const & /*pose*/
)
{
	if ( tag->getName() != "InterfaceScoreCalculator" ){
		utility_exit_with_message("This should be impossible");
	}
	if ( ! tag->hasOption("chains") ) utility_exit_with_message("'InterfaceScoreCalculator' requires 'chains' tag (comma separated chains to dock)");

	std::string const chains_str = tag->getOption<std::string>("chains");
	chains_= utility::string_split(chains_str, ',');

	/// Score Function ///
	if ( ! tag->hasOption("scorefxn") ) utility_exit_with_message("'HighResDocker' requires 'scorefxn' tag");
	std::string scorefxn_name= tag->getOption<std::string>("scorefxn");
	score_fxn_= datamap.get< core::scoring::ScoreFunction * >( "scorefxns", scorefxn_name);
	assert(score_fxn_);

	if (tag->hasOption("native") ){
		std::string const & native_str= tag->getOption<std::string>("native");
		std::vector<std::string> natives_strs= utility::string_split(native_str, ',');
		std::string natives_str = utility::join(natives_strs, " ");

		native_ = new core::pose::Pose;
		core::import_pose::pose_from_pdb(*native_, natives_str);
	}
	else if ( basic::options::option[ basic::options::OptionKeys::in::file::native ].user()){
		std::string const & native_str= basic::options::option[ basic::options::OptionKeys::in::file::native ]().name();
		native_ = new core::pose::Pose;
		core::import_pose::pose_from_pdb(*native_, native_str);
	}
	if(datamap.has("scoringgrid","default"))
	{
		grid_manager_ = datamap.get<qsar::scoring_grid::GridManager *>("scoringgrid","default");
	}
}

void InterfaceScoreCalculator::apply(core::pose::Pose & pose) {
	protocols::jd2::JobOP job( protocols::jd2::JobDistributor::get_instance()->current_job() );
	add_scores_to_job(pose, job);
	append_ligand_docking_scores(pose, job);
}

void InterfaceScoreCalculator::add_scores_to_job(
	core::pose::Pose & pose,
	protocols::jd2::JobOP job
) const
{
	assert(score_fxn_);
	using namespace core::scoring;

	core::Real const tot_score = score_fxn_->score( pose );

	// Which score terms to use
	typedef utility::vector1<ScoreType> ScoreTypeVec;
	ScoreTypeVec score_types;
	for(int i = 1; i <= n_score_types; ++i) {
		ScoreType ii = ScoreType(i);
		if ( score_fxn_->has_nonzero_weight(ii) ) score_types.push_back(ii);
	}


	for(ScoreTypeVec::iterator ii = score_types.begin(), end_ii = score_types.end(); ii != end_ii; ++ii) {
		job->add_string_real_pair(name_from_score_type(*ii), score_fxn_->get_weight(*ii) * pose.energies().total_energies()[ *ii ]);
	}
	job->add_string_real_pair(name_from_score_type(core::scoring::total_score), tot_score);
}


/// @brief For multiple ligands, append ligand docking scores for each ligand
void
InterfaceScoreCalculator::append_ligand_docking_scores(
		core::pose::Pose const & after,
		protocols::jd2::JobOP job
) const
{
	std::vector<std::string>::const_iterator begin = chains_.begin();
	std::vector<std::string>::const_iterator end = chains_.end();
	for( ; begin != end ; ++begin){
		InterfaceScoreCalculator_tracer.Debug << "appending ligand: "<< *begin << std::endl;
		assert( core::pose::has_chain(*begin, after));
		core::Size jump_id = core::pose::get_jump_id_from_chain(*begin, after);
		append_interface_deltas(jump_id, job, after, score_fxn_);
		if(native_){
			assert( core::pose::has_chain(*begin, *native_));
			append_ligand_docking_scores(jump_id, after, job);
		}
	}
}

/// @brief Scores to be output that aren't normal scorefunction terms.
void
InterfaceScoreCalculator::append_ligand_docking_scores(
	core::Size jump_id,
	core::pose::Pose const & after,
	protocols::jd2::JobOP job
) const {
		append_ligand_travel(jump_id, job, *native_, after);
		append_radius_of_gyration(jump_id, job, *native_);
		append_ligand_RMSD(jump_id, job, *native_, after);
		if(grid_manager_ != 0)
		{
			append_ligand_grid_scores(jump_id,job,after,grid_manager_);
		}
}



} //namespace ligand_docking
} //namespace protocols
