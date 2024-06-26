// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core//simple_metric/SimpleMetricFactory.cc
/// @brief  Implementation of the class for instantiating arbitrary SimpleMetrics
///         from a string --> SimpleMetricCreator map
/// @author Jared Adolf-Bryfogle (jadolfbr@gmail.com)

// Unit headers
#include <core//simple_metrics/SimpleMetricFactory.hh>

// Package headers
#include <core/simple_metrics/SimpleMetric.hh>
#include <core/simple_metrics/SimpleMetricCreator.hh>
#include <core/simple_metrics/util.hh>

// Utility headers
#include <utility/excn/Exceptions.hh>
#include <utility/tag/Tag.hh>
#include <utility/tag/XMLSchemaGeneration.fwd.hh>
#include <utility/tag/xml_schema_group_initialization.hh>

// Basic headers
#include <basic/citation_manager/CitationManager.hh>

namespace core {
namespace simple_metrics {


SimpleMetricFactory::SimpleMetricFactory():
	utility::SingletonBase< SimpleMetricFactory >(),
	creator_map_()
{}

void
SimpleMetricFactory::factory_register( SimpleMetricCreatorOP creator )
{
	if ( creator_map_.find( creator->keyname() ) != creator_map_.end() ) {
		std::string const err_msg = "Factory Name Conflict: Two or more SimpleMetricCreators registered with the name " + creator->keyname();
		utility_exit_with_message(  err_msg );
	}
	creator_map_[ creator->keyname() ] = creator;
}

bool SimpleMetricFactory::has_type( std::string const & selector_type ) const
{
	return creator_map_.find( selector_type ) != creator_map_.end();
}

SimpleMetricOP
SimpleMetricFactory::new_simple_metric(
	std::string const & simple_metric_name,
	utility::tag::TagCOP tag,
	basic::datacache::DataMap & datamap
) const
{
	if ( ! has_type( simple_metric_name ) ) {
		std::string err_msg =  "No SimpleMetricCreator with the name '" + simple_metric_name + "' has been registered with the SimpleMetricFactory";
		throw CREATE_EXCEPTION(utility::excn::Exception,  err_msg );
	}
	auto iter = creator_map_.find( simple_metric_name );
	SimpleMetricOP new_simple_metric = iter->second->create_simple_metric();
	new_simple_metric->parse_my_tag( tag, datamap );

	//Register with the CitationManager:
	basic::citation_manager::CitationCollectionList citations;
	new_simple_metric->provide_citation_info( citations );
	basic::citation_manager::CitationManager::get_instance()->add_citations( citations );

	return new_simple_metric;
}


/// @brief Get the XML schema for a given simple metric.
/// @details Throws an error if the residue selector is unknown to Rosetta.
/// @author Vikram K. Mulligan (vmullig@uw.edu)
void
SimpleMetricFactory::provide_xml_schema(
	std::string const & metric_name,
	utility::tag::XMLSchemaDefinition & xsd
) const {
	if ( ! has_type( metric_name ) ) {
		std::string err_msg =  "No SimpleMetric with the name '" + metric_name + "' has been registered with the SimpleMetricFactory";
		throw CREATE_EXCEPTION(utility::excn::Exception,  err_msg );
	}
	auto iter = creator_map_.find( metric_name );
	iter->second->provide_xml_schema( xsd );
}

void
SimpleMetricFactory::define_simple_metric_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const{
	try{
		utility::tag::define_xml_schema_group(
			creator_map_,
			simple_metric_xml_schema_group_name(),
			& core::simple_metrics::complex_type_name_for_simple_metric,
			xsd );
	} catch( utility::excn::Exception const & e ) {
		throw CREATE_EXCEPTION(utility::excn::Exception,  "Could not generate an XML Schema for SimpleMetrics from SimpleMetricsFactory; offending class"
			" must call protocols::simple_metric::complex_type_name_for_simple_metric when defining"
			" its XML Schema\n" + e.msg() );
	}
}

std::string
SimpleMetricFactory::simple_metric_xml_schema_group_name(){
	return "simple_metric";
}

/// @brief Get a human-readable listing of the citations for a given simple metric, by metric name.
/// @details Returns an empty string if there are no citations.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
std::string
SimpleMetricFactory::get_citation_humanreadable(
	std::string const & metric_name
) const {
	using namespace basic::citation_manager;
	CitationCollectionList citations;
	auto const iter = creator_map_.find( metric_name );
	runtime_assert_string_msg( iter != creator_map_.end(), "Error in SimpleMetricFactory::get_citation_humanreadable(): Could not find simple metric \"" + metric_name + "\"!" );
	SimpleMetricOP new_simple_metric = iter->second->create_simple_metric();
	runtime_assert_string_msg( new_simple_metric != nullptr, "Error in SimpleMetricFactory::get_citation_humanreadable(): Could not instantiate " + metric_name + "!" );
	new_simple_metric->provide_citation_info(citations);
	if ( citations.empty() ) return "";
	std::ostringstream ss;
	ss << "References and author information for the " << metric_name << " simple metric:" << std::endl;
	ss << std::endl;
	basic::citation_manager::CitationManager::get_instance()->write_all_citations_and_unpublished_author_info_from_list_to_stream( citations, ss );
	return ss.str();
}

/// @brief Access the map of name->creator.  Used only for testing.
SimpleMetricFactory::CreatorMap const &
SimpleMetricFactory::creator_map() const {
	return creator_map_;
}

} //namespace simple_metrics
} //namespace core
