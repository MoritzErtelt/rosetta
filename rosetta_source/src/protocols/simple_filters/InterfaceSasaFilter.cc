// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/simple_filters/InterfaceSasaFilter.cc
/// @brief
/// @author Sarel Fleishman (sarelf@u.washington.edu), Jacob Corn (jecorn@u.washington.edu)

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <protocols/simple_filters/InterfaceSasaFilter.hh>
#include <protocols/simple_filters/InterfaceSasaFilterCreator.hh>
#include <core/pose/Pose.hh>
#include <core/id/AtomID_Map.hh>
#include <core/conformation/Residue.hh>
#include <core/chemical/AtomType.hh>
#include <protocols/rigid/RigidBodyMover.hh>
#include <basic/MetricValue.hh>
#include <utility/tag/Tag.hh>
#include <protocols/moves/DataMap.hh>
// Jacob
#include <utility/string_util.hh>
#include <core/pose/symmetry/util.hh>
#include <core/conformation/symmetry/SymmetryInfo.hh>
#include <core/pose/symmetry/util.hh>

// Project Headers
#include <core/types.hh>


namespace protocols {
namespace simple_filters {

static basic::Tracer TR( "protocols.simple_filters.InterfaceSasaFilter" );

protocols::filters::FilterOP
InterfaceSasaFilterCreator::create_filter() const { return new InterfaceSasaFilter; }

std::string
InterfaceSasaFilterCreator::keyname() const { return "Sasa"; }


InterfaceSasaFilter::InterfaceSasaFilter() :
	Filter( "Sasa" ),
	lower_threshold_( 0.0 ),
	hydrophobic_( false ),
	polar_( false ),
	upper_threshold_(100000000000.0),
	jumps_(),
	sym_dof_names_()
{
	jump(1);
}

InterfaceSasaFilter::InterfaceSasaFilter( core::Real const lower_threshold, bool const hydrophobic/*=false*/, bool const polar/*=false*/, core::Real upper_threshold, std::string sym_dof_names ) :
	Filter( "Sasa" ),
	lower_threshold_( lower_threshold ),
	hydrophobic_( hydrophobic ),
	polar_( polar ),
	upper_threshold_(upper_threshold),
	jumps_(),
	sym_dof_names_()
{
	if (sym_dof_names != "")
	{
		this->sym_dof_names(sym_dof_names);
	}
	else
	{
		jump(1);
	}
}

InterfaceSasaFilter::~InterfaceSasaFilter(){}

filters::FilterOP
InterfaceSasaFilter::clone() const{
	return new InterfaceSasaFilter( *this );
}

filters::FilterOP
InterfaceSasaFilter::fresh_instance() const{
	return new InterfaceSasaFilter;
}

void
InterfaceSasaFilter::parse_my_tag( utility::tag::TagPtr const tag, moves::DataMap &, filters::Filters_map const &,moves::Movers_map const &, core::pose::Pose const & )
{
	lower_threshold_ = tag->getOption<core::Real>( "threshold", 800 );
	upper_threshold_ = tag->getOption<core::Real>( "upper_threshold", 1000000);

	std::string specified_jumps = tag->getOption< std::string >( "jump", "" );
	std::string specified_sym_dof_names = tag->getOption< std::string >( "sym_dof_names", "" );

	if(specified_jumps != "" && specified_sym_dof_names != "")
	{
		TR.Error << "Can not specify 'jump' and 'sym_dof_names' in InterfaceSasaFilter" << tag << std::endl;
		utility_exit_with_message( "Can not specify 'jump' and 'sym_dof_names' in InterfaceSasaFilter" );
	}
	else if(specified_jumps != "")
	{
		// Populate jumps_ with str->int converstions of the jump list.
		TR.Debug << "Reading jump list: " << specified_jumps << std::endl;

		jumps_.resize(0);

		utility::vector1<std::string> jump_strings = utility::string_split( specified_jumps, ',' );
		for(core::Size i = 1; i <= jump_strings.size(); ++i)
		{
			jumps_.push_back( boost::lexical_cast<core::Size>(jump_strings[i]));
		}

		sym_dof_names_.resize(0);
	}
	else if(specified_sym_dof_names != "")
	{
		TR.Debug << "Reading sym_dof_name list: " << specified_sym_dof_names << std::endl;

		jumps_.resize(0);
		sym_dof_names_ = utility::string_split( specified_sym_dof_names, ',');
	}
	else
	{
		TR.Debug << "Defaulting to jump 1. " << std::endl;

		jump(1);
	}

	hydrophobic_ = tag->getOption<bool>( "hydrophobic", false );
	polar_ = tag->getOption<bool>( "polar", false );

	if( polar_ && hydrophobic_ )
	{
		TR.Error << "Polar and hydrophobic flags specified in Sasa filter: " << tag << std::endl;
		utility_exit_with_message( "Polar and hydrophobic flags specified in Sasa filter." );
	}

	if( ( polar_ || hydrophobic_ ) && (jumps_.size() != 1 || jumps_[0] != 1))
	{
		TR.Error << "Only total sasa is supported across a jump other than 1. Remove polar and hydrophobic flags and try again: " << tag << std::endl;
		utility_exit_with_message( "Only total sasa is supported across a jump other than 1. Remove polar and hydrophobic flags and try again." );
	}

	TR.Debug << "Parsed Sasa Filter: <Sasa" <<
		" threshold=" << lower_threshold_ <<
		" upper_threshold=" << upper_threshold_ <<
		" jump=";
	std::copy(jumps_.begin(), jumps_.end(), std::ostream_iterator<core::Size>(TR.Debug, ","));
	TR.Debug <<
		" sym_dof_names=";
	std::copy(sym_dof_names_.begin(), sym_dof_names_.end(), std::ostream_iterator<std::string>(TR.Debug, ","));
	TR.Debug <<
		" hydrophobic=" << hydrophobic_ <<
		" polar=" << polar_ <<
		" />" << std::endl;
}

bool
InterfaceSasaFilter::apply( core::pose::Pose const & pose ) const {
	core::Real const sasa( compute( pose ) );

	TR<<"sasa is "<<sasa<<". ";
	if( sasa >= lower_threshold_ && sasa <= upper_threshold_ ){
		TR<<"passing." <<std::endl;
		return true;
	}
	else {
		TR<<"failing."<<std::endl;
		return false;
	}
}

void
InterfaceSasaFilter::report( std::ostream & out, core::pose::Pose const & pose ) const {
	core::Real const sasa( compute( pose ));
	out<<"Sasa= "<< sasa<<'\n';
}

core::Real
InterfaceSasaFilter::report_sm( core::pose::Pose const & pose ) const {
	core::Real const sasa( compute( pose ));
	return( sasa );
}

void
InterfaceSasaFilter::jump( core::Size const jump )
{
	jumps_.resize(0);
	jumps_.push_back(jump);
	sym_dof_names_.resize(0);
}

void
InterfaceSasaFilter::add_jump( core::Size const jump )
{
	jumps_.push_back(jump);
}

void InterfaceSasaFilter::jumps( utility::vector1<core::Size> const jumps )
{
	jumps_ = jumps;
}

void
InterfaceSasaFilter::sym_dof_names( std::string const sym_dof_names )
{
	sym_dof_names_ = utility::string_split(sym_dof_names, ',');
}

void
InterfaceSasaFilter::add_sym_dof_name( std::string const sym_dof_name )
{
	sym_dof_names_.push_back(sym_dof_name);
}

void
InterfaceSasaFilter::sym_dof_names( utility::vector1<std::string> const sym_dof_names )
{
	sym_dof_names_ = sym_dof_names;
}

core::Real
InterfaceSasaFilter::compute( core::pose::Pose const & pose ) const {
	using namespace core::pose::metrics;
	using basic::MetricValue;
	using namespace core;
	using namespace protocols::moves;

	core::pose::Pose split_pose( pose );
	std::set<core::Size> sym_aware_jump_ids;

	for (Size i = 1; i <= sym_dof_names_.size(); i++)
	{
		sym_aware_jump_ids.insert(core::pose::symmetry::sym_dof_jump_num( split_pose, sym_dof_names_[i] ));
	}

	for (Size i = 1; i <= jumps_.size(); i++)
	{
		sym_aware_jump_ids.insert(core::pose::symmetry::get_sym_aware_jump_num( split_pose, jumps_[i] ));
	}

	runtime_assert( !sym_aware_jump_ids.empty() );

	foreach (Size sym_aware_jump_id, sym_aware_jump_ids)
	{
		TR.Debug << "Moving jump id: " << sym_aware_jump_id << std::endl;

		protocols::rigid::RigidBodyTransMoverOP translate( new protocols::rigid::RigidBodyTransMover( split_pose, sym_aware_jump_id ) );
		translate->step_size( 1000.0 );
		translate->apply( split_pose );
	}

	runtime_assert( !hydrophobic_ || !polar_ );
	if( !hydrophobic_ && !polar_ )
	{
		MetricValue< core::Real > mv_sasa;

		pose.metric( "sasa", "total_sasa", mv_sasa);
		core::Real const bound_sasa( mv_sasa.value() );
		TR.Debug << "Bound sasa: " << bound_sasa << std::endl;

		split_pose.metric( "sasa", "total_sasa", mv_sasa );
		core::Real const unbound_sasa( mv_sasa.value() );
		TR.Debug << "Unbound sasa: " << unbound_sasa << std::endl;

		if( core::pose::symmetry::is_symmetric( pose ))
		{

			core::conformation::symmetry::SymmetryInfoCOP sym_info = core::pose::symmetry::symmetry_info(pose);
			core::Real const buried_sasa( (unbound_sasa - bound_sasa) /(sym_info->subunits()));

			TR.Debug << "Normalizing calculated sasa by subunits: " << sym_info->subunits() << std::endl;
			TR.Debug << "Buried sasa: " << buried_sasa << std::endl;

			return( buried_sasa );
		} else {
			core::Real const buried_sasa(unbound_sasa - bound_sasa);

			TR.Debug << "Buried sasa: " << buried_sasa << std::endl;
			return( buried_sasa );
		}
	}
	else{
		MetricValue< id::AtomID_Map< core::Real > > atom_sasa;
		pose.metric( "sasa_interface", "delta_atom_sasa", atom_sasa );
		core::Real polar_sasa( 0.0 ), hydrophobic_sasa( 0.0 );
		for( core::Size pos(1); pos<=pose.total_residue(); ++pos ){
			for( core::Size atomi( 1 ); atomi <= atom_sasa.value().n_atom( pos ); ++atomi ){
				core::Real const atomi_delta_sasa( atom_sasa.value()( pos, atomi ) );
				core::conformation::Residue const pos_rsd( pose.residue( pos ) );
				core::chemical::AtomType const atom_type( pos_rsd.atom_type( atomi ) );
				bool const is_polar( atom_type.is_donor() || atom_type.is_acceptor() || atom_type.is_polar_hydrogen() );
				if( is_polar ) polar_sasa += atomi_delta_sasa;
				else hydrophobic_sasa += atomi_delta_sasa;
			}
		}
		if( hydrophobic_ ) return hydrophobic_sasa;
		if( polar_ ) return polar_sasa;
	}
	utility_exit_with_message( "Execution should never have reached this point." );
	return( 0 );
}

}
}
