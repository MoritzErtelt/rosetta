// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/ligand_docking/GrowLigand.cc
///
/// @brief
/// @Gordon Lemmon

#include <protocols/ligand_docking/GrowLigand.hh>
#include <protocols/ligand_docking/GrowLigandCreator.hh>

#include <protocols/ligand_docking/LigandDesign.hh> // For helper functions.  Refactor this
#include <core/pose/chains_util.hh>


#include <protocols/moves/Mover.hh>

#include <core/types.hh>

#include <basic/Tracer.hh>

// option key includes


#include <core/conformation/Residue.hh>
#include <core/chemical/ResidueTypeFinder.hh>
#include <core/chemical/ChemicalManager.hh>
#include <utility/tag/Tag.hh>

//Auto Headers
#include <core/pose/Pose.hh>
#include <utility/vector1.hh>

//Auto Headers
#include <utility/excn/Exceptions.hh>
#include <core/conformation/Conformation.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

#include <numeric/random/random.hh> // AUTO IWYU For rg, RandomGenerator


namespace protocols {
namespace ligand_docking {

static basic::Tracer grow_ligand_tracer( "protocols.ligand_docking.GrowLigand", basic::t_debug );




GrowLigand::GrowLigand():
	Mover("GrowLigand"),
	chain_("")
{
	set_fragments();
}

GrowLigand::GrowLigand(std::string const & chain):
	Mover("GrowLigand"),
	chain_(chain)
{
	set_fragments();
}

GrowLigand::GrowLigand(GrowLigand const & /*that*/) = default;

GrowLigand::~GrowLigand() = default;

void
GrowLigand::set_fragments(){
	core::chemical::ChemicalManager *cm= core::chemical::ChemicalManager::get_instance();
	core::chemical::ResidueTypeSetCOP rsd_set= cm->residue_type_set( core::chemical::FA_STANDARD );
	// following may not work -- however, i could not find test cases with FRAGMENT residue_types.
	core::chemical::ResidueTypeCOPs fragment_types = core::chemical::ResidueTypeFinder( *rsd_set ).base_property( core::chemical::FRAGMENT ).get_all_possible_residue_types();
	grow_ligand_tracer<< fragment_types.size()<< " fragment_types"<< std::endl;

	for ( core::chemical::ResidueTypeCOP fragment_type : fragment_types ) {
		fragments_.push_back( utility::pointer::make_shared< core::conformation::Residue >(*fragment_type, true) );
		grow_ligand_tracer<< "frag_name: "<< fragment_type->name()<< std::endl;
	}
}

protocols::moves::MoverOP GrowLigand::clone() const {
	return utility::pointer::make_shared< GrowLigand >( *this );
}

protocols::moves::MoverOP GrowLigand::fresh_instance() const {
	return utility::pointer::make_shared< GrowLigand >();
}


/// @brief parse XML (specifically in the context of the parser/scripting scheme)
void
GrowLigand::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap & /*datamap*/
)
{
	if ( tag->hasOption("chain") ) {
		chain_ = tag->getOption<std::string>("chain");
	} else {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "HeavyAtom filter needs a 'chain' option");
	}
}

void
GrowLigand::apply( core::pose::Pose & pose )
{
	debug_assert(!fragments_.empty());
	debug_assert(chain_.size() == 1);

	utility::vector1<core::Size> unconnected_residues;
	{
		core::Size const chain_id= core::pose::get_chain_id_from_chain(chain_, pose);
		core::Size const start = pose.conformation().chain_begin(chain_id);
		core::Size const end = pose.conformation().chain_end(chain_id);
		unconnected_residues=find_unconnected_residues(pose, start, end);
	}

	core::Size grow_from = numeric::random::rg().random_element(unconnected_residues);
	core::Size grow_from_connection= random_connection(pose.residue(grow_from).get_self_ptr());
	core::conformation::ResidueCOP growth = numeric::random::rg().random_element(fragments_);
	core::Size growth_connection= random_connection(growth);
	bool const build_ideal_geometry= true;
	bool const start_new_chain = false;
	bool const lookup_bond_length = true;

	pose.append_residue_by_bond(
		*growth,
		build_ideal_geometry,
		growth_connection,
		grow_from,
		grow_from_connection,
		start_new_chain,
		lookup_bond_length
	);
	return;
}

void GrowLigand::fragments_to_string() const{
	for ( core::Size i=1; i <= fragments_.size(); ++i ) {
		auto  begin= fragments_.begin();
		for ( ; begin != fragments_.end(); ++begin ) {
			//core::conformation::Residue const & res= *begin;
			//core::Size connect_id= begin;
			//std::string name= begin->name();
			grow_ligand_tracer<< "atom_type, res_name, connection"<<" "<< (*begin)->name() << " "<< std::endl;
		}
	}
}

void GrowLigand::add_scores_to_job(
	core::pose::Pose & /*pose*/
)
{
}

std::string GrowLigand::get_name() const {
	return mover_name();
}

std::string GrowLigand::mover_name() {
	return "GrowLigand";
}

void GrowLigand::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute::required_attribute("chain", xs_string, "Chain ID.");
	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "Randomly connects a fragment from the library to the growing ligand.", attlist );
}

std::string GrowLigandCreator::keyname() const {
	return GrowLigand::mover_name();
}

protocols::moves::MoverOP
GrowLigandCreator::create_mover() const {
	return utility::pointer::make_shared< GrowLigand >();
}

void GrowLigandCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	GrowLigand::provide_xml_schema( xsd );
}


} // namespace ligand_docking
} // namespace protocols
