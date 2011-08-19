// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file   protocols/moves/StructureRestrictor.hh
///
/// @brief  lookup relevant chains for a structure in a table.
/// @author Matthew O'Meara

/// This should probably be a pilot app, but the way Rosetta Scripts
/// is set up, it can't be in the pilot apps


#ifndef INCLUDED_protocols_moves_StructureRestrictor_hh
#define INCLUDED_protocols_moves_StructureRestrictor_hh

#include <protocols/moves/StructureRestrictor.fwd.hh>

#include <protocols/moves/Mover.hh>
#include <utility/tag/Tag.fwd.hh>

#include <map>
#include <string>

using namespace std;
using namespace core;
  using namespace pose;

namespace protocols{
namespace moves{


class StructureRestrictor : public protocols::moves::Mover {

public:
  StructureRestrictor();

  StructureRestrictor( string const & name);

  StructureRestrictor(StructureRestrictor const & src);

  virtual ~StructureRestrictor();

  virtual MoverOP fresh_instance() const;

  virtual MoverOP clone() const;

	virtual std::string get_name() const { return "StructureRestrictor"; }


  // So this this can be called from RosettaScripts
  void
  parse_my_tag(
	       TagPtr const tag,
	       protocols::moves::DataMap & /*data*/,
	       Filters_map const & /*filters*/,
	       protocols::moves::Movers_map const & /*movers*/,
	       Pose const & /*pose*/ );

  void
  setup_relevant_chains(
			string const & relevant_chains_fname,
			map<string const, string const> & chain_map
			);

  // this is a hack because poses do not have canonical names!
  string pose_name(Pose const & pose);

  void apply( Pose& pose );

private:
  map<string const, string const> chain_map;
  string relevant_chains_fname;
  bool initialized;
};

}//moves
}//protocols

#endif //INCLUDED_protocols_moves_StructureRestrictor_FWD_HH
