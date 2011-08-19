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
#include <protocols/ligand_docking/Rotate.hh>
#include <protocols/ligand_docking/RotateCreator.hh>

#include <protocols/ligand_docking/grid_functions.hh>
#include <protocols/geometry/RB_geometry.hh>
#include <protocols/moves/RigidBodyMover.hh>
#include <protocols/moves/DataMap.hh>
#include <core/pose/util.hh>
#include <core/pose/Pose.hh>
#include <core/conformation/Conformation.hh>
#include <core/conformation/Residue.hh>

#include <core/scoring/rms_util.tmpl.hh>

#include <protocols/qsar/scoring_grid/GridManager.hh>
#include <protocols/qsar/qsarMap.hh>

// Utility Headers
#include <numeric/random/random.hh>
#include <utility/exit.hh>
#include <basic/Tracer.hh>
#include <core/types.hh>
#include <algorithm>
#include <utility/tag/Tag.hh>

//Auto Headers
#include <core/grid/CartGrid.hh>

using basic::T;
using basic::Error;
using basic::Warning;

namespace protocols {
namespace ligand_docking {

static basic::Tracer rotate_tracer("protocols.ligand_docking.ligand_options.rotate", basic::t_debug);

std::string
RotateCreator::keyname() const
{
	return RotateCreator::mover_name();
}

protocols::moves::MoverOP
RotateCreator::create_mover() const {
	return new Rotate;
}

std::string
RotateCreator::mover_name()
{
	return "Rotate";
}

Ligand_info::Ligand_info():atr(0), rep(0), jump(){}

Ligand_info::Ligand_info(core::conformation::ResidueCOPs const residues, int atr, int rep):
		residues(residues), atr(atr), rep(rep){}

Ligand_info::Ligand_info(core::conformation::ResidueCOPs const residues, std::pair<int,int> scores, core::kinematics::Jump jump):
		residues(residues), atr(scores.first), rep(scores.second), jump(jump){}


bool Ligand_info::operator<(Ligand_info const ligand_info) const{
	return ( rep < ligand_info.rep || (rep == ligand_info.rep && atr < ligand_info.atr ) );
}
bool Ligand_info::operator<(std::pair<int,int> const scores) const{
	return rep < scores.second || (rep == scores.second && atr < scores.first);
}
core::conformation::ResidueCOPs
const & Ligand_info::get_residues() const{
	return residues;
}

///@brief
Rotate::Rotate():
		//utility::pointer::ReferenceCount(),
		Mover("Rotate"),
		grid_manager_(0)
{}

Rotate::Rotate(Rotate const & that):
		//utility::pointer::ReferenceCount(),
		protocols::moves::Mover( that ),
		rotate_info_(that.rotate_info_)
{}

Rotate::~Rotate() {}

protocols::moves::MoverOP Rotate::clone() const {
	return new Rotate( *this );
}

protocols::moves::MoverOP Rotate::fresh_instance() const {
	return new Rotate;
}

std::string Rotate::get_name() const{
	return "Rotate";
}

///@brief parse XML (specifically in the context of the parser/scripting scheme)
void
Rotate::parse_my_tag(
		utility::tag::TagPtr const tag,
		protocols::moves::DataMap & data_map,
		protocols::filters::Filters_map const & /*filters*/,
		protocols::moves::Movers_map const & /*movers*/,
		core::pose::Pose const & /*pose*/
)
{
	if ( tag->getName() != "Rotate" ){
		utility_exit_with_message("This should be impossible");
	}
	if ( ! tag->hasOption("chain") ) utility_exit_with_message("'Rotate' mover requires 'chain' tag");
	if ( ! tag->hasOption("distribution") ) utility_exit_with_message("'Rotate' mover requires 'distribution' tag");
	if ( ! tag->hasOption("degrees") ) utility_exit_with_message("'Rotate' mover requires 'degrees' tag");
	if ( ! tag->hasOption("cycles") ) utility_exit_with_message("'Rotate' mover requires 'cycles' tag");

	if(data_map.has("scoringgrid","default"))
	{
		grid_manager_ = data_map.get<qsar::scoring_grid::GridManager *>("scoringgrid","default");
	}

	rotate_info_.chain = tag->getOption<std::string>("chain");
	{
		std::string distribution_str= tag->getOption<std::string>("distribution");
		rotate_info_.distribution= get_distribution(distribution_str);
	}
	rotate_info_.degrees = tag->getOption<core::Size>("degrees");
	rotate_info_.cycles = tag->getOption<core::Size>("cycles");
}

void Rotate::apply(core::pose::Pose & pose){
	rotate_info_.chain_id= core::pose::get_chain_id_from_chain(rotate_info_.chain, pose);
	rotate_info_.jump_id= core::pose::get_jump_id_from_chain_id(rotate_info_.chain_id, pose);

	core::Vector const center = protocols::geometry::downstream_centroid_by_jump(pose, rotate_info_.jump_id);
	if(grid_manager_ == 0)
	{
		utility::pointer::owning_ptr<core::grid::CartGrid<int> > const grid = make_atr_rep_grid(pose, center);
		rotate_ligand(grid, pose);// move ligand to a random point in binding pocket
	}else
	{
		if(grid_manager_->is_qsar_map_attached())
		{
			//core::conformation::ResidueOP residue = new core::conformation::Residue(pose.residue(begin));
			//qsar::qsarMapOP qsar_map(new qsar::qsarMap("default",residue));
			//qsar_map->fill_with_value(1);
			//grid_manager_->set_qsar_map(qsar_map);
		}
		//grid_manager_->initialize_all_grids(center);
		//grid_manager_->update_grids(pose,center);
	}
}

///@brief for n random rotations, randomly pick one from among the best scoring set of diverse poses
void Rotate::rotate_ligand(
		utility::pointer::owning_ptr<core::grid::CartGrid<int> >  const & grid,
		core::pose::Pose & pose
) {
	if(rotate_info_.degrees == 0) return;

	protocols::moves::RigidBodyMoverOP mover;
	if(rotate_info_.distribution == Uniform){
		mover= new protocols::moves::RigidBodyRandomizeMover( pose, rotate_info_.jump_id, protocols::moves::partner_downstream, rotate_info_.degrees, rotate_info_.degrees);
	}
	else if(rotate_info_.distribution == Gaussian){
		mover= new protocols::moves::RigidBodyPerturbMover ( rotate_info_.jump_id, rotate_info_.degrees, 0 /*translate*/);
	}
	core::Size chain_begin = pose.conformation().chain_begin(rotate_info_.chain_id);

	utility::vector1< Ligand_info> ligands= create_random_rotations(grid, mover, chain_begin, pose);

	core::Size const jump_choice=  (core::Size) numeric::random::RG.random_range(1, ligands.size());
	{
		pose.set_jump(rotate_info_.jump_id, ligands[jump_choice].jump);

		core::conformation::ResidueCOPs::iterator begin= ligands[jump_choice].residues.begin();
		core::conformation::ResidueCOPs::iterator const end= ligands[jump_choice].residues.begin();
		for(; begin != end; ++begin, ++chain_begin){
			pose.replace_residue(chain_begin, **begin, false /*orient backbone*/);// assume rotamers are oriented?
		}

	}
}

void Rotate::rotate_ligand(core::pose::Pose & pose)
{
	if(rotate_info_.degrees == 0) return;

	protocols::moves::RigidBodyMoverOP mover;
	if(rotate_info_.distribution == Uniform){
		mover= new protocols::moves::RigidBodyRandomizeMover( pose, rotate_info_.jump_id, protocols::moves::partner_downstream, rotate_info_.degrees, rotate_info_.degrees);
	}
	else if(rotate_info_.distribution == Gaussian){
		mover= new protocols::moves::RigidBodyPerturbMover ( rotate_info_.jump_id, rotate_info_.degrees, 0 /*translate*/);
	}
	//core::Size chain_begin = pose.conformation().chain_begin(rotate_info_.chain_id);

}

utility::vector1< Ligand_info>
Rotate::create_random_rotations(
		utility::pointer::owning_ptr<core::grid::CartGrid<int> > const & grid,
		protocols::moves::RigidBodyMoverOP const mover,
		core::Size const begin,
		core::pose::Pose & pose
)const{
	core::Size const end = pose.conformation().chain_end(rotate_info_.chain_id);
	core::Size const heavy_atom_number= core::pose::num_heavy_atoms(begin, end, pose);
	core::pose::Pose local_pose= pose;
	local_pose.remove_constraints();
	core::Vector const center = protocols::geometry::downstream_centroid_by_jump(local_pose, rotate_info_.jump_id);

	utility::vector1< Ligand_info> ligands;  ///TODO make this a set.  The set should check for another pose with a similar RMSD.
	core::Size max_diversity= 5*core::pose::num_chi_angles(begin, end, local_pose)+1; // who knows why?  copied from Ian's code.

	Ligand_info best=create_random_rotation(grid, mover, center, begin, end, local_pose);// first case;
	add_ligand_conditionally(best, ligands, heavy_atom_number);
	for(core::Size i=1; i< rotate_info_.cycles && ligands.size() <= max_diversity ; ++i){
		Ligand_info current =create_random_rotation(grid, mover, center, begin, end, local_pose);
		if (current < best){
			best= current;
		}
		add_ligand_conditionally(current, ligands, heavy_atom_number);
	}
	if(ligands.empty()){
		ligands.push_back(best);
	}
	return ligands;
}

/*
utility::vector1<Ligand_info> Rotate::create_random_rotations(
		protocols::moves::RigidBodyMoverOP const,
		core::Size const begin,
		core::pose::Pose & pose) const
{
	core::Size const end = pose.conformation().chain_end(rotate_info_.chain_id);
	//core::Size const heavy_atom_number= ligand_options::num_heavy_atoms(begin, end, pose);
	core::pose::Pose local_pose= pose;
	local_pose.remove_constraints();
	core::Vector const center = protocols::geometry::downstream_centroid_by_jump(local_pose, rotate_info_.jump_id);
	utility::vector1< Ligand_info> ligands;  ///TODO make this a set.  The set should check for another pose with a similar RMSD.
	//core::Size max_diversity= 5*ligand_options::num_chi_angles(begin, end, local_pose)+1; // who knows why?  copied from Ian's code.


}
*/

Ligand_info Rotate::create_random_rotation(
		utility::pointer::owning_ptr<core::grid::CartGrid<int> > const & grid,
		protocols::moves::RigidBodyMoverOP const mover,
		core::Vector const center,
		core::Size const begin,
		core::Size const end,
		core::pose::Pose & local_pose
) const{
	apply_rotate(mover, local_pose, center);
	rb_grid_rotamer_trials_atr_rep(*grid, local_pose, begin, end);
	core::kinematics::Jump jump= local_pose.jump(rotate_info_.jump_id);
	std::pair<int, int> const scores= get_rb_atr_and_rep_scores(*grid, local_pose, begin, end);
	core::conformation::ResidueCOPs const residues= core::pose::get_chain_residues(local_pose, rotate_info_.chain_id);
	return Ligand_info(residues, scores, jump);
}
/*
Ligand_info Rotate::create_random_rotation(
		protocols::moves::RigidBodyMoverOP const mover,
		core::Vector const center,
		core::Size const begin,
		core::Size const end,
		core::pose::Pose & local_pose
) const{
	apply_rotate(mover,local_pose,center);

}
*/
void add_ligand_conditionally(
		Ligand_info const & ligand_info,
		utility::vector1< Ligand_info> & ligands,
		core::Size const heavy_atom_number
){
	if(
			check_score(ligand_info, heavy_atom_number)
			&& check_RMSD(ligand_info, heavy_atom_number, ligands)
	){
		ligands.push_back(ligand_info);
	}
}

void apply_rotate(
		protocols::moves::RigidBodyMoverOP mover,
		core::pose::Pose & pose,
		core::Vector const & center
){
	mover->apply(pose);
	pose.update_actcoords();///TODO Verify necessity
	mover->rot_center(center); // restore the old center so ligand doesn't walk away (really necessary?)
}

bool check_score(
		Ligand_info const ligand,
		core::Size const heavy_atom_number
){
	int const atr_threshold=0;
	int const rep_threshold=-(int) (0.85 * heavy_atom_number);
	return ligand.atr <= atr_threshold && ligand.rep <= rep_threshold;
}

bool check_RMSD(
		Ligand_info const ligand,
		core::Size const heavy_atom_number,
		utility::vector1< Ligand_info> const & ligands
){
	assert(heavy_atom_number > 0);

	// This next parameter is a wild heuristic guesses that seem OK for the Meiler x-dock set.
	core::Real const diverse_rms = 0.65 * std::sqrt((double) heavy_atom_number);

	utility::vector1< Ligand_info >::const_iterator iter= ligands.begin();
	utility::vector1< Ligand_info >::const_iterator const end= ligands.end();

	for(; iter+1 != end; ++iter){
		core::conformation::ResidueCOPs const & these_residues= ligand.get_residues();
		core::conformation::ResidueCOPs const & compare_residues= iter->get_residues();
		runtime_assert(these_residues.size() == compare_residues.size());

		core::Real const rms = (compare_residues.size() == 1) ///TODO write multi_residue automorphic fxn.
				? core::scoring::automorphic_rmsd(*these_residues[1], *compare_residues[1], false)
				:core::scoring::rmsd_no_super(these_residues, compare_residues, core::scoring::is_ligand_heavyatom_residues);

		if (rms < diverse_rms) return false;
	}
	return true;
}

} //namespace ligand_docking
} //namespace protocols
