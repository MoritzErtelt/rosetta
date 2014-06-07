// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/protocols/environment/LocalPosition.cc
/// @author Justin Porter

// Unit Headers
#include <core/environment/LocalPosition.hh>

// Package headers

// Project headers
#include <utility/string_util.hh>


// tracer
#include <basic/Tracer.hh>

// C++ Headers
#include <boost/lexical_cast.hpp>

// ObjexxFCL Headers

static basic::Tracer tr("protocols.environment.LocalPosition", basic::t_info);

namespace core {
namespace environment {

LocalPosition::LocalPosition():
  ReferenceCount(),
  label_( "" ),
  position_( 0 )
{}

LocalPosition::LocalPosition( std::string const& label, core::Size const& position ):
  ReferenceCount(),
  label_( label ),
  position_( position )
{}

LocalPosition::LocalPosition( std::string const& comma_deliniated ):
  ReferenceCount()
{
  using core::Size;

  utility::vector1< std::string > str_split = utility::string_split( comma_deliniated, ',' );
  if( str_split.size() == 1 ){
    try {
      core::Size global_seqpos = boost::lexical_cast< core::Size >( str_split[1] );
      label( "BASE" );
      position( global_seqpos );
    } catch ( boost::bad_lexical_cast & ) {
      label( str_split[1] );
      position( 1 );
    }
  } else if( str_split.size() == 2 ){
    label( str_split[1] );
    position( boost::lexical_cast< core::Size >( str_split[2] ) );
  }
}

std::string const& LocalPosition::label() const {
  return label_;
}

core::Size const& LocalPosition::position() const {
  return position_;
}

void LocalPosition::label( std::string const& label ) {
  label_ = label;
}

void LocalPosition::position( core::Size position ){
  position_ = position;
}


bool LocalPosition::operator< ( LocalPosition const& other ) const {
  Size cmp = label_.compare( other.label_ );
  if( cmp == 0 ){
    return position_ < other.position_;
  } else if ( cmp > 0 ){
    return true;
  } else {
    return false;
  }
}

bool LocalPosition::operator==( LocalPosition const& other ) const {
  if( label_.compare( other.label_ ) == 0 &&
      position_ == other.position_ ){
    return true;
  } else {
    return false;
  }
}

bool LocalPosition::operator!=( LocalPosition const& other ) const {
  return !this->operator==( other );
}


std::ostream& operator<<( std::ostream& str, LocalPosition const& dof_passport ) {
  str << "( " << dof_passport.label() << ", " << dof_passport.position() << " )" ;
  return str;
}

} // environment
} // core
