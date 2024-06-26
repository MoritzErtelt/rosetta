// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   src/apps/pilot/delucasl/ligands_to_database.cc
/// @author Sam DeLuca
#include <basic/options/option.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>
#include <devel/init.hh>
#include <core/chemical/residue_io.hh>
#include <core/chemical/ResidueDatabaseIO.hh>
#include <core/chemical/ResidueType.hh>
#include <utility/sql_database/DatabaseSessionManager.fwd.hh>
#include <basic/database/sql_utils.hh>
#include <core/chemical/ChemicalManager.hh>
#include <core/chemical/ResidueTypeSet.fwd.hh>
#include <utility/vector1.hh>
#include <string>
#include <basic/options/option_macros.hh>

OPT_1GRP_KEY(String,ligand_import,params_database_name)
OPT_1GRP_KEY(String,ligand_import,params_database_pq_schema)
OPT_1GRP_KEY(String,ligand_import,database_mode)

int main(int argc, char*argv[])
{
	try {

		NEW_OPT(ligand_import::params_database_name,"name of the database to import params files into","");
		NEW_OPT(ligand_import::params_database_pq_schema,"postgreSQL schema of the database to import params files into","");
		NEW_OPT(ligand_import::database_mode,"the database mode to use, select mysql or sqlite3","sqlite3");

		devel::init(argc,argv);

		core::chemical::ResidueDatabaseIO residue_database_io;

		std::string database_name = basic::options::option[basic::options::OptionKeys::ligand_import::params_database_name];
		std::string database_pq_schema = basic::options::option[basic::options::OptionKeys::ligand_import::params_database_pq_schema];
		utility::sql_database::DatabaseMode::e database_mode(
			utility::sql_database::database_mode_from_name(
			basic::options::option[basic::options::OptionKeys::ligand_import::database_mode]));

		utility::sql_database::sessionOP db_session(
			basic::database::get_db_session(database_mode, database_name, database_pq_schema));
		residue_database_io.initialize(db_session);

		core::chemical::ResidueTypeSetCOP residue_types =
			core::chemical::ChemicalManager::get_instance()->residue_type_set("fa_standard");

		utility::vector1<std::string> const & res_file_paths(basic::options::option[basic::options::OptionKeys::in::file::extra_res_fa]());

		for ( std::string const & params_file_path : res_file_paths ) {
			core::chemical::MutableResidueTypeOP new_mut_residue_type = core::chemical::read_topology_file( params_file_path, residue_types );
			core::chemical::ResidueTypeCOP new_residue_type( core::chemical::ResidueType::make( *new_mut_residue_type ) );

			residue_database_io.write_residuetype_to_database("fa_standard",*new_residue_type,db_session);
		}
	}
catch (utility::excn::Exception const & e ) {
	e.display();
	return -1;
}
	return 0;
}
