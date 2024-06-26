// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @brief
/// @author jk + dj


#include <core/pose/Pose.hh>
#include <devel/init.hh>
#include <core/pose/PDBInfo.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <basic/options/util.hh>
#include <basic/options/option.hh>
#include <basic/options/option_macros.hh>
#include <basic/Tracer.hh>
#include <core/scoring/Energies.hh>
#include <core/scoring/EnergyGraph.hh>


// Utility Headers
#include <string>

#include <basic/options/keys/out.OptionKeys.gen.hh>

//Auto Headers
#include <core/import_pose/import_pose.hh>

#include <utility/excn/Exceptions.hh>

#include <fstream> // AUTO IWYU For operator<<, ostringstream, basic_ostream, basic_ostream:...


using namespace core;
using namespace core::scoring;
using namespace basic::options;
using namespace basic::options::OptionKeys;

OPT_KEY( String, apo_chain )

static basic::Tracer TR( "apps.pilot.david_recompute_score_and_rmsd.main" );

//set to store pdb info keys
std::set <std::string> interface;
//stores resid of the ligand residue

//mjo commenting out 'input_pose' because it is unused and causes a warning
void define_interface( core::pose::Pose & /*input_pose*/ ) {
}

/// General testing code
int
main( int argc, char * argv [] )
{
	try {

		NEW_OPT ( apo_chain, "Chain to find contacts on","A");
		devel::init(argc, argv);
		pose::Pose input_pose;
		std::string chain = option [ apo_chain ];

		//read in pdb file from command line
		std::string const input_pdb_name ( basic::options::start_file() );
		core::import_pose::pose_from_file( input_pose, input_pdb_name , core::import_pose::PDB_file);

		std::filebuf fb;
		std::stringstream filename;
		filename<<option[ OptionKeys::out::output_tag ]()<<".contacts";
		fb.open (filename.str().c_str(),std::ios::out);
		std::ostream os(&fb);

		scoring::ScoreFunctionOP scorefxn( get_score_function() );
		(*scorefxn)(input_pose);

		EnergyGraph & energy_graph(input_pose.energies().energy_graph());

		for ( int i = 1, resnum = input_pose.size(); i <= resnum; ++i ) {
			std::ostringstream tmp;
			tmp<< input_pose.pdb_info()->chain(i);
			std::string rch = tmp.str();
			//if (rch != chain) continue;
			//std::cout<<std::endl<<input_pose.pdb_info()->number(i)<<":";
			for ( utility::graph::Graph::EdgeListIter
					iru  = energy_graph.get_node( i )->edge_list_begin(),
					irue = energy_graph.get_node( i )->edge_list_end();
					iru != irue; ++iru ) {
				auto * edge( static_cast< EnergyEdge *> (*iru) );
				Size const j( edge->get_other_ind( i ) );
				//Size const j( edge->get_first_node_ind() );

				// the pair energies cached in the link
				EnergyMap const & emap( edge->fill_energy_map());
				Real const attr( emap[ fa_atr ] );
				//TR<<"\n"<<j<<": "<<attr<<"\n";
				if ( attr < -.2 ) {
					//std::cout<<input_pose.pdb_info()->number(j)<<input_pose.pdb_info()->chain(j)<<":";
					std::ostringstream tmp2;
					tmp2<< input_pose.pdb_info()->chain(j);
					std::string bch = tmp2.str();
					// create string id to store in set
					if ( bch != chain && rch == chain ) {
						std::ostringstream residuestream;
						//std::cout << rch<<" "<<bch<<" "<<chain<<std::endl;
						TR << "resi "<< input_pose.pdb_info()->number(i)<<" or ";

						residuestream << input_pose.pdb_info()->chain(i) << input_pose.pdb_info()->number(i);
						std::string res_id = residuestream.str();
						interface.insert(res_id);
						os<<res_id<<std::endl;
						break;
					}
				}
			}
		}


		TR << std::endl;


	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}

	return 0;

}


