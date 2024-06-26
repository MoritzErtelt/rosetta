// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/canonical_sampling/MetricRecorder.cc
///
/// @brief
/// @author


// Unit header or inline function header
#include <protocols/canonical_sampling/MetricRecorder.hh>
#include <protocols/canonical_sampling/MetricRecorderCreator.hh>
#include <protocols/canonical_sampling/TrajectoryRecorder.hh>
// Other project headers or inline function headers
#include <core/pose/Pose.hh>
//Gabe testing
//#include <core/pose/Pose.hh>
#include <core/types.hh>
//End gabe testing
#include <core/scoring/Energies.hh>
#include <protocols/jd2/util.hh>
#include <protocols/canonical_sampling/MetropolisHastingsMover.hh>
#include <protocols/canonical_sampling/TemperingBase.hh>
#include <protocols/moves/MonteCarlo.hh>
#include <core/pose/selection.hh>
#include <utility/tag/Tag.hh>

// External library headers

// C++ headers
#include <algorithm>
#include <sstream>

#include <core/id/TorsionID.hh>
#include <utility/vector0.hh>
#include <utility/vector1.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

#include <core/chemical/ResidueType.hh> // AUTO IWYU For ResidueType


// Operating system headers

// Forward declarations


namespace protocols {
namespace canonical_sampling {


using core::Size;


MetricRecorder::MetricRecorder() :
	stride_(1),
	cumulate_jobs_(false),
	cumulate_replicas_(false),
	prepend_output_name_(false),
	step_count_(0),
	last_flush_(0)
{}

MetricRecorder::~MetricRecorder() = default;

MetricRecorder::MetricRecorder(
	MetricRecorder const & other
) :
	protocols::canonical_sampling::ThermodynamicObserver(other),
	file_name_(other.file_name_),
	stride_(other.stride_),
	cumulate_jobs_(other.cumulate_jobs_),
	cumulate_replicas_(other.cumulate_replicas_),
	prepend_output_name_(other.prepend_output_name_),
	step_count_(other.step_count_),
	torsion_ids_(other.torsion_ids_),
	torsion_specs_(other.torsion_specs_),
	last_flush_(other.last_flush_)
{}

protocols::moves::MoverOP
MetricRecorder::clone() const
{
	return utility::pointer::make_shared< protocols::canonical_sampling::MetricRecorder >( *this );
}

protocols::moves::MoverOP
MetricRecorder::fresh_instance() const
{
	return utility::pointer::make_shared< MetricRecorder >();
}


void
MetricRecorder::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap & /* data */
)
{
	stride_ = tag->getOption< core::Size >( "stride", 100 );
	cumulate_jobs_ = tag->getOption< bool >( "cumulate_jobs", false );
	cumulate_replicas_ = tag->getOption< bool >( "cumulate_replicas", false );
	prepend_output_name_ = tag->getOption< bool >( "prepend_output_name", false );
	file_name_ = tag->getOption< std::string >( "filename", "metrics.txt" );

	utility::vector0< utility::tag::TagCOP > const subtags( tag->getTags() );

	for ( auto subtag : subtags ) {

		protocols::moves::MoverOP mover;

		if ( subtag->getName() == "Torsion" ) {
			TorsionSpec torsion;
			torsion.style = TorsionSpecStyle::Single;
			torsion.rsd = subtag->getOption< std::string >( "rsd" );
			torsion.type = subtag->getOption< std::string >( "type" );
			torsion.number = subtag->getOption< core::Size >( "torsion" );
			torsion.name = subtag->getOption< std::string >( "name", "" );
			if ( subtag->hasOption("name") ) torsion.name = subtag->getOptions().find("name")->second;

			add_torsion( torsion );

		} else if ( subtag->getName() == "AllChi" ) {
			TorsionSpec torsion;
			torsion.style = TorsionSpecStyle::AllChi;
			add_torsion( torsion );
		} else if ( subtag->getName() == "AllBB" ) {
			TorsionSpec torsion;
			torsion.style = TorsionSpecStyle::AllBB;
			add_torsion( torsion );
		} else {
			utility_exit_with_message("Parsed unknown metric type in MetricRecorder" + subtag->getName());
		}
	}
}

std::string const &
MetricRecorder::file_name() const
{
	return file_name_;
}

void
MetricRecorder::file_name(
	std::string const & file_name
)
{
	file_name_ = file_name;
}

core::Size
MetricRecorder::stride() const
{
	return stride_;
}

void
MetricRecorder::stride(
	core::Size stride
)
{
	stride_ = stride;
}

bool
MetricRecorder::cumulate_jobs() const
{
	return cumulate_jobs_;
}

void
MetricRecorder::cumulate_jobs(
	bool cumulate_jobs
)
{
	cumulate_jobs_ = cumulate_jobs;
}

bool
MetricRecorder::cumulate_replicas() const
{
	return cumulate_replicas_;
}

void
MetricRecorder::cumulate_replicas(
	bool cumulate_replicas
)
{
	cumulate_replicas_ = cumulate_replicas;
}

bool
MetricRecorder::prepend_output_name() const
{
	return prepend_output_name_;
}

void
MetricRecorder::prepend_output_name(
	bool prepend_output_name
)
{
	prepend_output_name_ = prepend_output_name;
}

void
MetricRecorder::add_torsion(
	core::id::TorsionID const & torsion_id,
	std::string const & name // = ""
)
{
	runtime_assert(torsion_id.valid());

	if ( name != "" ) {
		torsion_ids_.push_back( {torsion_id, name} );
	} else {
		torsion_ids_.push_back( {torsion_id, torsion_id.str()} );
	}

}

void
MetricRecorder::add_torsion(
	core::pose::Pose const & pose,
	std::string const & rsd,
	std::string const & type,
	core::Size torsion,
	std::string const & name // = ""
)
{
	core::Size parsed_rsd = core::pose::parse_resnum(rsd, pose);
	add_torsion( torsion_ids_, parsed_rsd, type, torsion, name );
}

void
MetricRecorder::add_torsion(
	utility::vector1<std::pair<core::id::TorsionID, std::string> > & torsion_ids,
	core::Size rsd,
	std::string type,
	core::Size torsion,
	std::string const & name // = ""
) const {

	std::transform(type.begin(), type.end(), type.begin(), toupper);
	core::id::TorsionType parsed_type;
	if ( type == "BB" ) {
		parsed_type = core::id::BB;
	} else if ( type == "CHI" ) {
		parsed_type = core::id::CHI;
	} else if ( type == "JUMP" ) {
		parsed_type = core::id::JUMP;
	} else {
		utility_exit_with_message("Unknown torsion type");
	}

	core::id::TorsionID torsion_id(rsd, parsed_type, torsion);
	runtime_assert( torsion_id.valid() );

	if ( name != "" ) {
		torsion_ids.push_back( {torsion_id,name} );
	} else {
		torsion_ids.push_back( {torsion_id,torsion_id.str()} );
	}
}

void
MetricRecorder::add_torsion( TorsionSpec const & torsion ) {
	torsion_specs_.push_back( torsion );
}


void
MetricRecorder::reset(
	core::pose::Pose const & pose,
	protocols::canonical_sampling::MetropolisHastingsMover const * metropolis_hastings_mover //= 0
)
{
	step_count_ = 0;
	recorder_stream_.close();
	update_after_boltzmann(pose, metropolis_hastings_mover);
}

void
MetricRecorder::update_after_boltzmann(
	core::pose::Pose const & pose,
	protocols::canonical_sampling::MetropolisHastingsMover const * metropolis_hastings_mover //= 0
)
{
	if ( recorder_stream_.filename() == "" ) {
		std::ostringstream file_name_stream;
		if ( prepend_output_name_ && !metropolis_hastings_mover ) {
			file_name_stream << protocols::jd2::current_output_name() << '_';
		}
		file_name_stream << file_name_;
		recorder_stream_.open(file_name_stream.str());
	}

	core::Size nstruct_index( protocols::jd2::jd2_used() ? protocols::jd2::current_nstruct_index() : 1 );
	std::string output_name( metropolis_hastings_mover ? metropolis_hastings_mover->output_name() : "" );

	core::Size replica = protocols::jd2::current_replica();

	TemperingBaseCOP tempering = nullptr;
	if ( metropolis_hastings_mover ) {
		tempering = utility::pointer::dynamic_pointer_cast< TemperingBase const >( metropolis_hastings_mover->tempering() );
	}

	// Get the parsed torsions, now that we have a pose.
	utility::vector1<std::pair<core::id::TorsionID, std::string> > torsion_ids = get_torsion_ids(pose);

	if ( step_count_ == 0 ) {

		// output header if not cumulating, replica exchange inactive, or this is the first replica
		if ( (!cumulate_jobs_ || nstruct_index == 1) && (!cumulate_replicas_ || replica <= 1) ) {

			recorder_stream_ << "Trial";
			if ( cumulate_jobs_ && output_name.length() ) recorder_stream_ << '\t' << "Job";
			if ( cumulate_replicas_ && replica ) recorder_stream_ << '\t' << "Replica";
			if ( tempering ) recorder_stream_ << '\t' << "Temperature";
			recorder_stream_ << '\t' << "Score";

			for ( core::Size i = 1; i <= torsion_ids.size(); ++i ) {
				recorder_stream_ << '\t' << torsion_ids[i].second;
			}

			recorder_stream_ << std::endl;
			recorder_stream_.flush();
		}
		last_flush_ = time(nullptr);
	}

	if ( step_count_ % stride_ == 0 ) {

		recorder_stream_ << step_count_;
		if ( cumulate_jobs_ && output_name.length() ) recorder_stream_ << '\t' << output_name;
		if ( cumulate_replicas_ && replica ) recorder_stream_ << '\t' << replica;
		if ( tempering ) recorder_stream_ << '\t' << metropolis_hastings_mover->monte_carlo()->temperature();
		recorder_stream_ << '\t' << pose.energies().total_energy();

		for ( core::Size i = 1; i <= torsion_ids.size(); ++i ) {
			recorder_stream_ << '\t' << pose.torsion(torsion_ids[i].first);
		}

		recorder_stream_ << std::endl;

		time_t now = time(nullptr);
		if ( now-last_flush_ > 10 ) {
			recorder_stream_.flush();
			last_flush_ = now;
		}
	}

	++step_count_;
}

void
MetricRecorder::apply(
	core::pose::Pose & pose
)
{
	update_after_boltzmann(pose, nullptr);
}

void
MetricRecorder::initialize_simulation(
	core::pose::Pose &,
	protocols::canonical_sampling::MetropolisHastingsMover const & metropolis_hastings_mover,
	core::Size //default=0; non-zero if trajectory is restarted
)
{
	std::string original_file_name(file_name());

	file_name(metropolis_hastings_mover.output_file_name(file_name(), cumulate_jobs_, cumulate_replicas_));

	reset(
		metropolis_hastings_mover.monte_carlo()->last_accepted_pose(),
		&metropolis_hastings_mover
	);

	file_name(original_file_name);
}

void
MetricRecorder::observe_after_metropolis(
	protocols::canonical_sampling::MetropolisHastingsMover const & metropolis_hastings_mover
)
{
	update_after_boltzmann(
		metropolis_hastings_mover.monte_carlo()->last_accepted_pose(),
		&metropolis_hastings_mover
	);
}

void
MetricRecorder::finalize_simulation(
	core::pose::Pose &,
	protocols::canonical_sampling::MetropolisHastingsMover const &
)
{
	recorder_stream_.close();
}

std::string MetricRecorder::get_name() const {
	return mover_name();
}

std::string MetricRecorder::mover_name() {
	return "MetricRecorder";
}

void MetricRecorder::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	TrajectoryRecorder::attributes_for_trajectory_recorder( attlist );
	attlist
		+ XMLSchemaAttribute::attribute_w_default( "prepend_output_name", xsct_rosetta_bool, "Prepend the current job output name to the file name", "false" );
	//Define subelements
	//Attribute lists for subelements

	AttributeList torsion_attributes;
	torsion_attributes
		+ XMLSchemaAttribute::required_attribute( "rsd", xs_string, "Residue this torsion belongs to" )
		+ XMLSchemaAttribute::required_attribute( "type", xs_string, "Type of this torsion (e.g. CHI )" )
		+ XMLSchemaAttribute::required_attribute( "torsion", xsct_non_negative_integer, "Which of the specified torsion angles are we modifying? E.g. 1 for chi1" )
		+ optional_name_attribute( "Unique identifier for this torsion" );
	AttributeList allchi_attributes; //no attributes in this one
	allchi_attributes
		+ optional_name_attribute( "unique identifier for this tag" );
	AttributeList allbb_attributes; //none in this one either
	allbb_attributes
		+ optional_name_attribute( "unique identifier for this tag" );
	XMLSchemaSimpleSubelementList subelements;
	subelements
		.add_simple_subelement( "Torsion", torsion_attributes, "Specifies a torsion angle to be recorded" )
		.add_simple_subelement( "AllChi", allchi_attributes, "Record all chi angles" )
		.add_simple_subelement( "AllBB", allbb_attributes, "Record all backbone torsion angles" );

	XMLSchemaComplexTypeGenerator ct_gen;
	ct_gen
		.add_attributes( attlist )
		.set_subelements_repeatable( subelements )
		.element_name( mover_name() )
		.complex_type_naming_func( & moves::complex_type_name_for_mover )
		.description( "Record numeric metrics to a tab-delimited text file" )
		.write_complex_type_to_schema( xsd );

}

std::string MetricRecorderCreator::keyname() const {
	return MetricRecorder::mover_name();
}

protocols::moves::MoverOP
MetricRecorderCreator::create_mover() const {
	return utility::pointer::make_shared< MetricRecorder >();
}

void MetricRecorderCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	MetricRecorder::provide_xml_schema( xsd );
}

utility::vector1<std::pair<core::id::TorsionID, std::string> >
MetricRecorder::get_torsion_ids( core::pose::Pose const & pose ) const {
	utility::vector1<std::pair<core::id::TorsionID, std::string> > torsion_ids = torsion_ids_;
	for ( TorsionSpec const & spec: torsion_specs_ ) {
		switch ( spec.style ) {
		case TorsionSpecStyle::Single :
			{
			core::Size parsed_rsd = core::pose::parse_resnum(spec.rsd, pose);
			add_torsion( torsion_ids, parsed_rsd, spec.type, spec.number, spec.name );
			break;
		}
		case TorsionSpecStyle::AllChi :
			{
			for ( Size i = 1; i <= pose.size(); ++i ) {
				for ( Size j = 1; j <= pose.residue_type(i).nchi(); ++j ) {
					std::ostringstream name_stream;
					name_stream << pose.residue_type(i).name3() << "_" << i << "_Chi" << j ;
					add_torsion( torsion_ids, i, "CHI", j, name_stream.str() );
				}
			}
			break;
		}
		case TorsionSpecStyle::AllBB :
			{
			for ( Size i = 1; i <= pose.size(); ++i ) {
				add_torsion(torsion_ids, i, "BB", 1, pose.residue_type(i).name3() + "_" + std::to_string(i) + "_phi");
				add_torsion(torsion_ids, i, "BB", 2, pose.residue_type(i).name3() + "_" + std::to_string(i) + "_psi");
				add_torsion(torsion_ids, i, "BB", 3, pose.residue_type(i).name3() + "_" + std::to_string(i) + "_omega");
			}
			break;
		}
		default :
			utility_exit_with_message("TorsionSpec style not recognized.");
		}
	}
	return torsion_ids;
}


} // namespace canonical_sampling
} // namespace protocols
