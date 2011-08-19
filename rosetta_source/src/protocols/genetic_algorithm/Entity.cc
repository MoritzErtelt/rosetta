// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file Entity.hh
/// @brief the unit employed/optimized by GeneticAlgorithm
/// @author ashworth

#include <protocols/genetic_algorithm/Entity.hh>

#include <core/types.hh>

#include <utility/exit.hh>
#include <utility/pointer/ReferenceCount.hh>
#include <utility/pointer/owning_ptr.hh>
// AUTO-REMOVED #include <utility/pointer/access_ptr.hh>
// AUTO-REMOVED #include <utility/vector1.hh>

#include <boost/functional/hash.hpp> // hash_range

#include <ObjexxFCL/format.hh>
using namespace ObjexxFCL;

#include <iostream>
#include <sstream>

//Auto Headers
#include <utility/vector1.fwd.hh>
#include <utility/pointer/access_ptr.fwd.hh>


namespace protocols {
namespace genetic_algorithm {

EntityElement::EntityElement() : index_( 0 ) {}
EntityElement::EntityElement( Size index ) : parent(), index_( index ) {}
EntityElement::EntityElement( std::string & word ) : parent()
{
	if ( word == "" ) {
		index_ = 0;
	} else {
		Size colon_pos = word.find( ":" );
		if ( colon_pos == std::string::npos ) {
			std::cerr << "ERROR: Could not find colon (:) in input word while constructing EntityElement.  word='" << word << "'" << std::endl;
			utility_exit_with_message( "EntityElement constructor failure" );
		} else {
			std::string indstring = word.substr( 0, colon_pos );
			std::istringstream iss( indstring );
			iss >> index_;
			if ( colon_pos + 1 < word.size() ) {
				word = word.substr( colon_pos + 1 );
			} else {
				word = "";
			}
		}
	}
}

EntityElement::~EntityElement() {}

void EntityElement::index( Size ind ) { index_ = ind; }
EntityElement::Size EntityElement::index() const { return index_; }

bool
EntityElement::operator <  ( EntityElement const & rhs ) const {
	return index_ < rhs.index_;
}

bool
EntityElement::operator == ( EntityElement const & rhs ) const {
	return index_ == rhs.index_;
}

EntityElement const &
EntityElement::operator =  ( EntityElement const & rhs )
{
	if ( this != &rhs ) {
		index_ = rhs.index_;
	}
	return *this;
}

std::string EntityElement::to_string() const
{
	std::ostringstream oss;
	oss << name() << ":" << index_ << ":";
	return oss.str();
}

EntityElementCreator::~EntityElementCreator() {}

/// Entity Element Factory

EntityElementFactory * EntityElementFactory::instance_( 0 );

EntityElementFactory * EntityElementFactory::get_instance()
{
	if ( ! instance_ ) {
		instance_ = new EntityElementFactory;
	}
	return instance_;
}

EntityElementOP
EntityElementFactory::element_from_string( std::string const & entity_element_name )
{
	//std::cout << "entity element name: " << entity_element_name << std::endl;

	std::string type;
	std::string desc;
	core::Size colon_pos = entity_element_name.find( ":" );

	//std::cout << "colon pos: " << colon_pos << std::endl;

	if ( colon_pos != std::string::npos ) {
		type = entity_element_name.substr( 0, colon_pos );
		if ( colon_pos + 1 < entity_element_name.size() ) {
			desc = entity_element_name.substr( colon_pos + 1 );
		}
	} else {
		utility_exit_with_message( "Invalid EntityElementCreator input; could not locate a colon in the string \"" + entity_element_name + "\"" );
	}

	//std::cout << "type: " << type << " desc: " << desc << std::endl;

	CreatorMap::const_iterator iter = creators().find( type );
	if ( iter == creators().end() ) {
		std::cerr << "ERROR: Could not find EntityElement type " << type << " in EntityElementFactory" << std::endl;
		utility_exit_with_message( "EntityElementFactory type lookup failure" );
	}
	return iter->second->new_entity( desc );
}

/*
void EntityElementFactory::register_creator( EntityElementCreatorOP creator )
{
	CreatorMap::const_iterator iter = creators_.find( creator->entity_type() );
	if ( iter != creators_.end() ) {
		std::cerr << "ERROR: EntityElement type " << creator->entity_type() << " has already been registered in the EntityElementFactory" << std::endl;
		utility_exit_with_message( "EntityElementFactory type duplication failure" );
	}
	creators_[ creator->entity_type() ] = creator;
}*/


std::string EntityElementFactory::factory_name() const {
	return "EntityElementFactory";
}

EntityElementFactory::EntityElementFactory() {}

Entity::Entity() :
	utility::pointer::ReferenceCount(),
	fitness_(0.),
	fitness_valid_(false)
{}

Entity::Entity( std::string const & line )
{
	std::istringstream linestream( line );
	if ( !read_checkpoint(linestream) ) utility_exit_with_message( "invalid string " + line );
}

Entity::Entity( Entity const & entity ) :
	utility::pointer::ReferenceCount(),
	traits_( entity.traits_.size() ),
	fitness_( entity.fitness_ ),
	fitness_valid_( entity.fitness_valid_ )
{
	for ( Size ii = 1; ii <= traits_.size(); ++ii ) {
		if ( entity.traits_[ ii ] ) {
			traits_[ ii ] = entity.traits_[ ii ]->clone();
		} else {
			traits_[ ii ] = 0;
		}
	}
}


Entity const &
Entity::operator = ( Entity const & rhs ) {
	if ( this != & rhs ) {
		if ( traits_.size() == rhs.traits_.size() ) {
			for ( Size ii = 1; ii <= traits_.size(); ++ii ) {
				if ( rhs.traits_[ ii ] && traits_[ ii ] ) {
					(*traits_[ ii ]) = (*rhs.traits_[ ii ] ); // polymorphic assignment operator; assumption: both traits are the same type!
				} else {
					traits_[ ii ] = rhs.traits_[ ii ]->clone();
				}
			}
		} else {
			traits_.resize( rhs.traits_.size() );
			for ( Size ii = 1; ii <= traits_.size(); ++ii ) {
				if ( rhs.traits_[ ii ] ) {
					traits_[ ii ] = rhs.traits_[ ii ]->clone();
				} else {
					traits_[ ii ] = 0;
				}
			}
		}
	}
	return *this;
}


Entity::~Entity(){}

Entity::OP Entity::clone() const { return new Entity(*this); }

void Entity::set_traits_size(
	core::Size size/*,
	EntityElementOP element*/
)
{
	traits_.resize(size);
	/*
	for ( Size ii = 1; ii <= size; ++ii ) {
		traits_[ ii ] = element->fresh_instance();
		traits_[ ii ]->index( ii );
	}*/
	fitness_valid_ = false;
}
void Entity::set_traits( EntityElements const & traits )
{
	if ( traits_.size() == traits.size() ) {
		for ( Size ii = 1; ii <= traits_.size(); ++ii ) {
			if ( traits_[ ii ] ) {
				*traits_[ ii ] = *traits[ ii ]; /// Assumption: both traits are the same type.
			} else {
				traits_[ ii ] = traits[ ii ]->clone();
			}
		}
	} else {
		traits_.resize( traits.size() );
		for ( Size ii = 1; ii <= traits_.size(); ++ii ) {
			traits_[ ii ] = traits[ ii ]->clone();
		}
	}
	fitness_valid_ = false;
}

void Entity::set_entity_element(
	core::Size index,
	EntityElementOP element
)
{
	traits_[ index ] = element->clone();
	fitness_valid_ = false;
}

EntityElements const &
Entity::traits() const {
	return traits_;
}

void Entity::set_fitness( core::Real val ) {
	fitness_ = val;
	fitness_valid_ = true;
}

core::Real Entity::fitness() const {
	return fitness_;
}

bool Entity::fitness_valid() const {
	return fitness_valid_;
}

bool Entity::operator == ( Entity const & other ) const
{
	if ( traits_.size() != other.traits_.size() ) return false;

	for ( Size ii = 1; ii <= traits_.size(); ++ii ) {
		if ( ! traits_[ ii ] && ! other.traits_[ ii ] ) {
			continue; /// apl -- this really shouldn't happen
		}
		if ( ! traits_[ ii ] || ! other.traits_[ ii ] ) {
			return false; /// apl -- this really shouldn't happen
		}
		if ( ! ( *traits_[ ii ] == *other.traits_[ ii ] ) ) {
			return false;
		}
	}
	return true;
}

bool Entity::operator < ( Entity const & other ) const
{
	return ( fitness_ < other.fitness() );
}

void Entity::show( std::ostream & os ) const
{
	os << "Entity with traits:";
	EntityElements const & seq( this->traits() );
	for ( EntityElements::const_iterator
			it( seq.begin() ), end( seq.end() );
			it != end; ++it ) {
		os << " " << (*it)->to_string();
	}
	os << " and fitness " << fmt::F(6,3,this->fitness());
}

void
Entity::write_checkpoint(
	std::ostream & os
) const
{
	os << "traits";
	for ( EntityElements::const_iterator
			key( traits_.begin() ), end( traits_.end() ); key != end; ++key ) {
		os << " " << (*key)->to_string();
	}
	os << " fitness " << fitness_;
}

bool
Entity::read_checkpoint(
	std::istream & is
)
{
	std::string word;
	if (!(is >> word)) return false;
	if ( word != "traits" ) return false;
	while ( is >> word ) {
		if ( word == "fitness" ) break;
		traits_.push_back( EntityElementFactory::get_instance()->element_from_string( word ) );
	}
	if ( is >> fitness_) {
		fitness_valid_ = true;
		return true;
	}
	return false;
}

std::ostream & operator << ( std::ostream & os, Entity const & entity )
{
	entity.show(os);
	return os;
}


} // namespace genetic_algorithm
} // namespace protocols

