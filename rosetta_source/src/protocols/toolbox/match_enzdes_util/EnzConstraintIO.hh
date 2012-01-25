// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file IO-functionality for enzyme Constraints
/// @brief
/// @author Florian Richter, floric@u.washington.edu


#ifndef INCLUDED_protocols_toolbox_match_enzdes_util_EnzConstraintIO_hh
#define INCLUDED_protocols_toolbox_match_enzdes_util_EnzConstraintIO_hh


// Unit headers
// AUTO-REMOVED #include <core/scoring/constraints/ConstraintSet.fwd.hh>
#include <protocols/toolbox/match_enzdes_util/EnzConstraintIO.fwd.hh>


// AUTO-REMOVED


#ifdef WIN32
	#include <protocols/toolbox/match_enzdes_util/MatchConstraintFileInfo.hh>
	#include <protocols/toolbox/match_enzdes_util/EnzConstraintIO.hh>
	#include <core/scoring/constraints/Constraints.hh>
	#include <protocols/toolbox/match_enzdes_util/EnzConstraintParameters.hh>
#endif

#include <core/scoring/ScoreFunction.fwd.hh>

// Project headers
#include <core/chemical/ResidueTypeSet.fwd.hh>
#include <core/pose/Pose.fwd.hh>
// AUTO-REMOVED #include <core/pack/task/PackerTask.fwd.hh>
#include <core/id/SequenceMapping.fwd.hh>
#include <core/types.hh>


// Utility Headers
#include <utility/pointer/ReferenceCount.hh>
#include <utility/vector1.fwd.hh>
// AUTO-REMOVED #include <set>

#include <protocols/toolbox/match_enzdes_util/MatchConstraintFileInfo.fwd.hh>
#include <utility/vector1.hh>



//Utility Headers

// C++ Headers

namespace protocols {
namespace toolbox {
namespace match_enzdes_util {


////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


//interface class to process enzyme design constraints format, links information in
//constraint file containg constraint parameters and in pdb file containing
//the relevant residue numbers
//it also checks whether the information that was gathered from two different locations
//is consistent and then adds a constraint set to an input pose

class EnzConstraintIO : public utility::pointer::ReferenceCount{


public:

	EnzConstraintIO (core::chemical::ResidueTypeSetCAP src_restype_set);
	~EnzConstraintIO();

	static EnzConstraintIO* get_instance();

	void
	read_enzyme_cstfile(std::string fname );


	toolbox::match_enzdes_util::MatchConstraintFileInfoListCOP
	mcfi_list( core::Size block ) const;

	core::Size
	num_mcfi_lists() const {
		return mcfi_lists_.size(); }

	void
	add_constraints_to_pose(
		core::pose::Pose & pose,
		core::scoring::ScoreFunctionCOP scofx,
		bool accept_blocks_missing_header
	);


	/// @brief BE CAREFUL when using this function, it generates constraints
	/// @brief without clearing the internal data structures and reading in
	/// @brief the information in the pdb REMARKs
	/// @brief if you're unsure use the above one
	void
	add_constraints_to_pose_for_block_without_clearing_and_header_processing(
		core::pose::Pose & pose,
		core::scoring::ScoreFunctionCOP scofx,
		core::Size cst_block
	) const;


	/// @brief convenience function that will add constraints to the pose if they have
	/// @brief been previously generated. BE CAREFUL when using this function, it relies on the
	/// @brief pose having the same residue types at the same constrained positions as in the pose
	/// @brief that was originally used to generate the constraints. If in doubt, it's safer to
	/// @brief regenerate the constraints before adding (i.e. use the above add_constraints_to_pose
	/// @brief function.)
	void
	add_pregenerated_constraints_to_pose(
		core::pose::Pose & pose,
 		core::scoring::ScoreFunctionCOP scofx
	) const;

	void
	remove_constraints_from_pose(
		core::pose::Pose & pose,
		bool const keep_covalent,
		bool const fail_on_constraints_missing
	) const;


	void
	remove_constraints_from_pose_for_block(
		core::pose::Pose & pose,
		core::Size cst_block,
		bool const fail_on_constraints_missing
	) const;


	void
	remove_position_from_template_res_for_block(
		core::pose::Pose & pose,
		core::Size pos,
		core::Size cst_block
	) const;

	void
	remove_position_from_template_res(
		core::pose::Pose & pose,
		core::Size pos
	) const;

	void
	process_pdb_header(
		core::pose::Pose & pose,
		bool accept_missing_blocks
	);

	/// @brief are constraints specified for this position?
	bool
	contains_position( core::pose::Pose const & pose, core::Size const seqpos ) const;

	/// @brief are the constraints specified at this position
	/// mediated through backbone interactions only?
	bool
	is_backbone_only_cst( core::pose::Pose const & pose, core::Size const seqpos ) const;

	void
	update_pdb_remarks_for_backbone_params(
		core::pose::Pose & pose )const ;

	utility::vector1< std::string >
	allowed_res_name3_at_position( core::pose::Pose const & pose, core::Size const seqpos ) const;

	void
	show_cst_definitions() const;

	/// @brief changing the constrained residues if the sequence length changes
	void
	remap_resid( core::id::SequenceMapping const & smap );

	void
	set_position_for_missing_res_in_parameter_block(
		core::pose::Pose & pose,
		core::Size cst_block,
		core::Size respos
	) const;

	void
	clear_active_pose_constraints_for_block(
		core::pose::Pose & pose,
		core::Size cst_block
	) const;

	void
	set_external_position_for_resA_in_parameter_block(
		core::Size cst_block,
		core::Size respos
	);

	void
	set_external_position_for_resB_in_parameter_block(
		core::Size cst_block,
		core::Size respos
	);
	/*
	void
	setup_favor_native_constraints(
		core::pose::Pose & pose,
		core::pack::task::PackerTaskCOP task,
		core::pose::Pose const & native_pose
	);

	void
	remove_favor_native_constraints(
		core::pose::Pose & pose
	);
	*/
	EnzConstraintParametersCOP
	enz_cst_params( core::Size block) const;

	utility::vector1< EnzConstraintParametersCOP >
	enz_cst_params_missing_in_pose( core::pose::Pose const & pose ) const;

	utility::vector1< core::Size >
	ordered_constrained_positions( core::pose::Pose const & pose) const;

	core::Size
	mcfi_lists_size() const;

	//MatchConstraintFileInfoListCOP
	//mcfi_list( Size index ) const;

	core::Size
	enz_cst_params_size() { return cst_pairs_.size(); }

protected:

	utility::vector1< EnzConstraintParametersOP > cst_pairs_; // contains information about the residue pair constraints

private:

	//void
	//clear_pose_specific_data();

	//void
	//clear_pose_specific_data_for_block( core::Size cst_block );

	void
	generate_pose_specific_data(
		core::pose::Pose & pose,
		core::scoring::ScoreFunctionCOP scofx
	) const;

	void
	generate_pose_specific_data_for_block(
		core::pose::Pose & pose,
		core::scoring::ScoreFunctionCOP scofx,
		core::Size cst_block
	) const;

	utility::vector1< toolbox::match_enzdes_util::MatchConstraintFileInfoListOP > mcfi_lists_;

	core::chemical::ResidueTypeSetCAP restype_set_;
	//bool cst_pair_data_consistent_;

	//utility::vector1< core::scoring::constraints::ConstraintCOP > favor_native_constraints_;

	//a static version for generic access
	static EnzConstraintIOOP generic_instance_;

};  // class EnzConstraintIO


}
} //toolbox
} //protocols

#endif
