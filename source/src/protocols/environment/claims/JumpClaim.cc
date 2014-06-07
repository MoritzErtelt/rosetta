// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file JumpClaim
/// @brief Claims access to a torsional angle.
/// @author Justin Porter

// Unit Headers
#include <protocols/environment/claims/JumpClaim.hh>

// Package Headers
#include <core/environment/LocalPosition.hh>
#include <core/environment/DofPassport.hh>

#include <protocols/environment/claims/EnvClaim.hh>

#include <protocols/environment/ClaimingMover.hh>
#include <protocols/environment/ProtectedConformation.hh>

// Project Headers
#include <core/id/TorsionID.hh>

#include <core/pose/util.hh>

// ObjexxFCL Headers

// Utility headers
#include <basic/Tracer.hh>

// C++ headers

// option key includes

static basic::Tracer tr("protocols.environment.JumpClaim",basic::t_info);

namespace protocols {
namespace environment {
namespace claims {

using core::environment::LocalPosition;

JumpClaim::JumpClaim( ClaimingMoverOP owner,
                      std::string const& jump_label,
                      LocalPosition const& jpos1,
                      LocalPosition const& jpos2,
                      LocalPosition const& cutp ):
  EnvClaim( owner ),
  label_( jump_label ),
  pos1_( jpos1 ),
  pos2_( jpos2 ),
  cut_( cutp ),
  atoms_( std::make_pair( "", "" ) ),
  physical_cut_( false ),
  create_vrt_p1_( false ),
  create_vrt_p2_( false ),
  stubs_intra_residue_( false ),
  c_str_( EXCLUSIVE ),
  i_str_( DOES_NOT_CONTROL )
{}

void JumpClaim::yield_elements( core::environment::FoldTreeSketch const&, ResidueElements& elements ) const {
  // this will create virtual residues to target with the jump if those labels don't exist.
  // that could be because there's actual sequence with that name, or it's already a vrt.
  if( create_vrt_p1_ ){
    ResidueElement e1;
    e1.label = pos1().label();
    e1.allow_duplicates = true;
    elements.push_back( e1 );
  }

  if( create_vrt_p2_ ){
    ResidueElement e2;
    e2.label = pos2().label();
    e2.allow_duplicates = true;
    elements.push_back( e2 );
  }
}

void JumpClaim::yield_elements( core::environment::FoldTreeSketch const&, JumpElements& elements ) const {
  JumpElement e;

  e.p1 = pos1();
  e.p2 = pos2();
  e.label = label();
  e.atom1 = atoms().first;
  e.atom2 = atoms().second;
  e.force_stub_intra_residue = stubs_intra_residue();
  e.has_physical_cut = physical();

  elements.push_back( e );
}

void JumpClaim::yield_elements( ProtectedConformationCOP const& conf, DOFElements& elements ) const {
  for( core::Size rb_i = core::id::RB1; rb_i <= core::id::RB6; ++rb_i ){
    DOFElement e;

    e.id = core::id::DOF_ID( conf->jump_atom_id( (int) conf->annotations()->resolve_jump( label() ) ),
                             core::id::DOF_Type( rb_i ) );

    e.i_str = i_str_;
    e.c_str = c_str_;
    e.owner.operator=( owner() );

    elements.push_back( e );
  }
}

void JumpClaim::yield_elements( core::environment::FoldTreeSketch const&, CutElements& elements ) const {
  if( cut_position() != LocalPosition( "", 0 ) ){
    CutElement e;
    e.p = cut_position();
    elements.push_back( e );
  }
}

void JumpClaim::set_atoms( std::string const& a1, std::string const& a2 ) {
  atoms_ = std::make_pair( a1, a2 );
}

void JumpClaim::strength( ControlStrength const& c_str, ControlStrength const& i_str ){
  c_str_ = c_str;
  i_str_ = i_str;
}

std::string const& JumpClaim::label() const {
  return label_;
}

LocalPosition const& JumpClaim::pos1() const {
  return pos1_;
}

LocalPosition const& JumpClaim::pos2() const {
  return pos2_;
}

void JumpClaim::create_vrt_if_necessary( bool setting ) {
  create_vrt_if_necessary( setting, setting );
}

void JumpClaim::create_vrt_if_necessary( bool setting_p1, bool setting_p2 ){
  create_vrt_p1_ = setting_p1;
  create_vrt_p2_ = setting_p2;
}

void JumpClaim::cut( claims::LocalPosition const& p ) {
  cut_ = p;
}

LocalPosition const& JumpClaim::cut_position() const {
  return cut_;
}

EnvClaimOP JumpClaim::clone() const {
  return new JumpClaim( *this );
}

std::string JumpClaim::str_type() const{
  return "Jump";
}

void JumpClaim::show( std::ostream& os ) const {
  os << str_type() << "(" << pos1() << "," << atoms().first << "->" << pos2()
     << "," << atoms().second << ") owned by a " << owner()->get_name();
}

} //claims
} //environment
} //protocols
