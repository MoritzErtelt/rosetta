// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file devel/denovo_design/task_operations/DesignBySecondaryStructure.hh
/// @brief Design residues with secondary structures that don't match the desired secondary structure
/// specified in the blueprint.
/// @author Tom Linsky (tlinsky@uw.edu)

#ifndef INCLUDED_devel_denovo_design_task_operations_designbysecondarystructure_hh
#define INCLUDED_devel_denovo_design_task_operations_designbysecondarystructure_hh

// unit headers
#include <devel/denovo_design/task_operations/DesignBySecondaryStructure.fwd.hh>

// protocol headers
#include <core/io/external/PsiPredInterface.fwd.hh>
#include <devel/denovo_design/task_operations/HighestEnergyRegion.hh>


// project headers
#include <protocols/ss_prediction/SS_predictor.fwd.hh>
#include <utility/tag/Tag.fwd.hh>
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

// Utility Headers
#include <core/types.hh>

namespace devel {
namespace denovo_design {
namespace task_operations {

class DesignBySecondaryStructureOperation : public HighestEnergyRegionOperation {
public:
	/// @brief default constructor
	DesignBySecondaryStructureOperation();

	/// @brief value constructor
	DesignBySecondaryStructureOperation( std::string const bp_file,
		std::string const cmd,
		bool const prevent_native,
		bool const prevent_bad_point_mutations );

	/// @brief copy constructor
	DesignBySecondaryStructureOperation( DesignBySecondaryStructureOperation const & rval );

	/// @brief destructor
	~DesignBySecondaryStructureOperation() override;

	/// @brief make clone
	core::pack::task::operation::TaskOperationOP clone() const override;

	/// @brief apply
	void apply( Pose const & pose, core::pack::task::PackerTask & task ) const override;

	/// @brief Runs the calculation and caches residues to design
	utility::vector1< core::Size >
	get_residues_to_design( core::pose::Pose const & pose ) const override;

	/// @brief Returns the name of the class
	std::string get_name() const override { return "SSPrediction"; }

	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );
	static std::string keyname() { return "DesignBySecondaryStructureOperation"; }

public:
	void parse_tag( utility::tag::TagCOP tag, basic::datacache::DataMap & ) override;

	/// @brief opens the passed blueprint file and determines the desired secondary structure
	void initialize_blueprint_ss( std::string const blueprint_file );

private:
	std::string blueprint_ss_; // the blueprint secondary structure
	std::string pred_ss_; // cache of the predicted secondary structure
	utility::vector1< core::Real > psipred_prob_; // cache of the predicted probability of secondary structure

	/// @brief If set, the amino acid residue already in the pose will be disallowed default=false
	bool prevent_native_aa_;
	/// @brief If set, all mutations at all positions will be scanned in one pass, and those that cause worse psipred secondary structure agreement will be disallowed (default=false)
	bool prevent_bad_point_mutants_;
	/// @brief the object which directly communicates with psipred and parses psipred output
	core::io::external::PsiPredInterfaceOP psipred_interface_;
	/// @brief the svm secondary structure predictor
	protocols::ss_prediction::SS_predictorOP ss_predictor_;
};


} // task_operations
} // denovo_design
} // devel

#endif
