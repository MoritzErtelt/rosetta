// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/ligand_docking/WriteLigandMolFile.cc
/// @brief  Ligand MolFile writer
/// @author Sam DeLuca

// MPI headers
#ifdef USEMPI
#include <mpi.h> //keep this first
#endif

#include <protocols/ligand_docking/WriteLigandMolFile.hh>
#include <protocols/ligand_docking/WriteLigandMolFileCreator.hh>
#include <utility/excn/Exceptions.hh>
#include <core/pose/Pose.hh>
#include <core/conformation/Conformation.hh>
#include <utility/file/file_sys_util.hh>
#include <protocols/jd2/util.hh>
#include <utility/tag/Tag.hh>
#include <utility/string_util.hh>
#include <utility/io/ozstream.hh>
#include <core/chemical/sdf/mol_writer.hh>
#include <core/pose/chains_util.hh>
#include <map>
#include <basic/Tracer.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

namespace protocols {
namespace ligand_docking {

static basic::Tracer write_ligand_tracer( "protocols.ligand_docking.WriteLigandMolFile" );




WriteLigandMolFile::WriteLigandMolFile() :
	Mover("WriteLigandMolFile")
{}

protocols::moves::MoverOP WriteLigandMolFile::clone() const
{
	return utility::pointer::make_shared< WriteLigandMolFile >(*this);
}

protocols::moves::MoverOP WriteLigandMolFile::fresh_instance() const
{
	return utility::pointer::make_shared< WriteLigandMolFile >();
}


void WriteLigandMolFile::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap &
)
{
	if ( !tag->hasOption("chain") ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "'WriteLigandMolFile' requires the option 'chain'");
	}

	if ( !tag->hasOption("directory") ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "'WriteLigandMolFile' requires the option 'directory'");
	}

	std::string hash_status = tag->getOption<std::string>("hash_file_names","false");
	if ( hash_status == "true" ) {
		hash_file_names_ = true;
	} else {
		hash_file_names_ = false;
	}

	chain_ = tag->getOption<std::string>("chain");
	directory_ = tag->getOption<std::string>("directory");
	prefix_ = tag->getOption<std::string>("prefix","");
	use_residue_name_ = tag->getOption< bool >("use_residue_name", false);

	if ( prefix_.empty() && ! use_residue_name_ ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "'WriteLigandMolFile' requires either the 'prefix' or the 'use_residue_name' options.");
	}

}

void WriteLigandMolFile::apply(core::pose::Pose & pose)
{
	core::chemical::sdf::MolWriter mol_writer("V2000");

	std::map< std::string, std::string > string_string_data( protocols::jd2::get_string_string_pairs_from_current_job() );
	std::map< std::string, core::Real > string_real_data( protocols::jd2::get_string_real_pairs_from_current_job() );

	std::map<std::string,std::string> job_data;

	for ( auto const & entry: string_string_data ) {
		job_data.insert( entry );
	}

	for ( auto const & entry: string_real_data ) {
		job_data.insert( std::make_pair( entry.first, utility::to_string(entry.second) ) );
	}

	mol_writer.set_job_data(job_data);

	core::Size chain_id = core::pose::get_chain_id_from_chain(chain_,pose);
	core::Size residue_index = pose.conformation().chain_begin(chain_id);
	core::conformation::ResidueCOP ligand_residue(pose.conformation().residue(residue_index).get_self_ptr());

	utility::file::create_directory(directory_);

	std::string output_file( directory_+"/"+prefix_ );

	if ( use_residue_name_ ) {
		if ( ! prefix_.empty() ) {
			output_file += "_";
		}
		output_file += ligand_residue->name();
	}

#ifdef USEMPI
	int mpi_rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);/* get current process id */
	std::string mpi_rank_string(utility::to_string(mpi_rank));
	output_file += "_" + mpi_rank_string;
#endif
	output_file += ".sdf.gz";

	utility::io::ozstream output;
	std::stringstream header;
	output.open_append_if_existed( output_file, header);

	write_ligand_tracer << "Writing ligand to " << output_file <<std::endl;
	mol_writer.output_residue(output,ligand_residue);

}

std::string WriteLigandMolFile::get_name() const {
	return mover_name();
}

std::string WriteLigandMolFile::mover_name() {
	return "WriteLigandMolFile";
}

void WriteLigandMolFile::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist
		+ XMLSchemaAttribute::attribute_w_default("hash_file_names", xs_string, "Seems to be unused.", "false")
		+ XMLSchemaAttribute::required_attribute("chain", xs_string, "The PDB chain ID of the ligand to be output.")
		+ XMLSchemaAttribute::required_attribute("directory", xs_string,
		"The directory all mol records will be output to. "
		"Directory will be created if it does not exist.")
		+ XMLSchemaAttribute("prefix", xs_string, "Set a file prefix for the output files.")
		+ XMLSchemaAttribute::attribute_w_default("use_residue_name", xsct_rosetta_bool, "Use the residue name as part of the prefix for the output files.", "false");
	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(),
		"Output a V2000 mol file record containing all atoms of the specified ligand "
		"chain and all data stored in the current Job for each processed pose", attlist );
}

std::string WriteLigandMolFileCreator::keyname() const {
	return WriteLigandMolFile::mover_name();
}

protocols::moves::MoverOP
WriteLigandMolFileCreator::create_mover() const {
	return utility::pointer::make_shared< WriteLigandMolFile >();
}

void WriteLigandMolFileCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	WriteLigandMolFile::provide_xml_schema( xsd );
}


}
}

