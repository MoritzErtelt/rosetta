// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file basic/database/schema_generator/ForeignKey.hh
/// @brief ForeignKey class for the schema generator framework
/// @author Tim Jacobs


#ifndef INCLUDED_basic_database_schema_generator_ForeignKey_HH
#define INCLUDED_basic_database_schema_generator_ForeignKey_HH

#include <utility/vector1.hh>
#include <utility/sql_database/DatabaseSessionManager.fwd.hh>

// The full header is necessary for Windows builds, and something
// arcane about the Windows PyRosetta build isn't responding to ifdefs.
// (It's also needed for IWYU to correctly parse things.)
#include <basic/database/schema_generator/Column.hh> // DO NOT AUTO-REMOVE

//C++ Headers
#include <string>

namespace basic {
namespace database {
namespace schema_generator {

class ForeignKey
{
public:

	ForeignKey(
		Column const & column,
		std::string const & reference_table,
		std::string const & reference_column);

	ForeignKey(
		Column const & column,
		std::string const & reference_table,
		std::string const & reference_column,
		bool defer);

	ForeignKey(
		Columns const & columns,
		std::string const & reference_table,
		utility::vector1<std::string> const & reference_columns,
		bool defer);

	Columns
	columns();

	std::string print(utility::sql_database::sessionOP) const;

private:

	Columns columns_;
	utility::vector1<std::string> reference_columns_;
	std::string reference_table_;

	// Some database backends can defer enforcing the primary key until
	// the end of the transaction, eg "DEFERABLE INITIALLY DEFFERED" in sqlite
	// and it is not possible with MySQL.
	bool defer_;
};


} // schema_generator
} // namespace database
} // namespace utility

#endif
