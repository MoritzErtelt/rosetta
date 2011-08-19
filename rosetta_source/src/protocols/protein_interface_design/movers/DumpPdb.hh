// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/protein_interface_design/movers/DumpPdb.hh
/// @author Sarel Fleishman (sarelf@u.washington.edu), Jacob Corn (jecorn@u.washington.edu)

#ifndef INCLUDED_protocols_protein_interface_design_movers_DumpPdb_hh
#define INCLUDED_protocols_protein_interface_design_movers_DumpPdb_hh

#include <core/types.hh>
#include <core/pose/Pose.fwd.hh>
#include <utility/tag/Tag.fwd.hh>
#include <protocols/filters/Filter.fwd.hh>
#include <protocols/moves/Mover.hh>
#include <core/scoring/ScoreFunction.hh>
#include <protocols/moves/DataMap.fwd.hh>

namespace protocols {
namespace protein_interface_design {
namespace movers {

/// @brief what you think
/// this can now be assimilated into DumpPdbMover
class DumpPdb : public protocols::moves::Mover
{
public:
	DumpPdb();
	DumpPdb( std::string const fname );
	virtual ~DumpPdb();
	void apply( core::pose::Pose & pose );
	virtual std::string get_name() const;
	protocols::moves::MoverOP clone() const {
		return( protocols::moves::MoverOP( new DumpPdb( *this ) ) );
	}
	protocols::moves::MoverOP fresh_instance() const { return protocols::moves::MoverOP( new DumpPdb ); }
	void set_scorefxn( core::scoring::ScoreFunctionOP scorefxn);
	void parse_my_tag( utility::tag::TagPtr const tag, protocols::moves::DataMap &, protocols::filters::Filters_map const &, protocols::moves::Movers_map const &, core::pose::Pose const & );
private:
	std::string fname_;
	/// @brief Dump a scored pdb?
	core::scoring::ScoreFunctionOP scorefxn_;
};


} // movers
} // protein_interface_design
} // protocols


#endif /*INCLUDED_protocols_protein_interface_design_movers_DumpPdb_HH*/
