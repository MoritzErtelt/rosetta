// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/protocols/rigid/UniformRigidBodyCM.cc
/// @author Justin R. Porter
/// @author Brian D. Weitzner
/// @author Oliver F. Lange

// Unit Headers
#include <protocols/rigid/UniformRigidBodyCM.hh>

// Package headers
#include <core/environment/DofPassport.hh>
#include <core/environment/LocalPosition.hh>

#include <protocols/environment/DofUnlock.hh>

#include <protocols/environment/claims/JumpClaim.hh>

#include <protocols/rigid/UniformRigidBodyCMCreator.hh>

// Project headers
#include <core/kinematics/MoveMap.hh>

#include <core/pose/util.hh>

//Utility Headers
#include <utility/excn/Exceptions.hh>
#include <utility/tag/Tag.hh>

#include <utility/string_util.hh>
#include <utility/vector1.hh>

#include <basic/datacache/DataMap.hh>

// tracer
#include <basic/Tracer.hh>

// C++ Headers

// ObjexxFCL Headers

namespace protocols {
namespace rigid {

std::string const DOCKJUMP_TAG = "_dockjump";

static basic::Tracer tr("protocols.rigid.UniformRigidBodyCM", basic::t_info);

using namespace core::environment;
using namespace protocols::environment;

// creator
std::string
UniformRigidBodyCMCreator::keyname() const {
  return UniformRigidBodyCMCreator::mover_name();
}

protocols::moves::MoverOP
UniformRigidBodyCMCreator::create_mover() const {
  return ClaimingMoverOP( new UniformRigidBodyCM );
}

std::string
UniformRigidBodyCMCreator::mover_name() {
  return "UniformRigidBodyCM";
}

UniformRigidBodyCM::UniformRigidBodyCM():
  ClaimingMover()
{}

UniformRigidBodyCM::UniformRigidBodyCM( std::string const& name,
                                        LocalPosition const& mobile_label,
                                        LocalPosition const& stationary_label,
                                        core::Real rotation_magnitude,
                                        core::Real translation_magnitude ):
  ClaimingMover(),
  name_( name ),
  mobile_label_( mobile_label ),
  stationary_label_( stationary_label ),
  rotation_mag_( rotation_magnitude ),
  translation_mag_( translation_magnitude )
{}

void UniformRigidBodyCM::passport_updated(){
  if( has_passport() ){
    SequenceAnnotationCOP ann = active_environment()->annotations();
    int dockjump = (int) ann->resolve_jump( name() + DOCKJUMP_TAG );

    // if the mover's not initialized or it's got a different dockjump
    if( !mover_ || mover_->jump_number() != dockjump ){
      mover_ = new UniformRigidBodyMover( dockjump, rotation_mag_, translation_mag_ );
    }
 } else {
    //configure a null moveset.
  }
}

void UniformRigidBodyCM::initialize( core::pose::Pose& pose ){
  DofUnlock activeation( pose.conformation(), passport() );
  mover_->apply( pose );
}

void UniformRigidBodyCM::apply( core::pose::Pose& pose ){
  DofUnlock activation( pose.conformation(), passport() );
  if( mover_ && passport()->has_jump_access( mover_->jump_number() ) ){
    mover_->apply( pose );
  } else if( !mover_ ){
    throw utility::excn::EXCN_NullPointer( "UniformRigidBodyCM found during apply. Did you forget to register this mover with the Environment?" );
  } else {
    tr.Warning << "UniformRigidBodyCM is skipping it's turn to apply a RigidBody move because it doesn't have "
               << " access to the requested dof, jump " << mover_->jump_number()
               << ". Are you sure this is expected behavior?" << std::endl;
  }
}

void UniformRigidBodyCM::parse_my_tag( utility::tag::TagCOP tag,
                                       basic::datacache::DataMap&,
                                       protocols::filters::Filters_map const&,
                                       protocols::moves::Movers_map const&,
                                       core::pose::Pose const& ) {

  mobile_label_ = LocalPosition( tag->getOption< std::string >( "mobile" ) );
  stationary_label_ = LocalPosition( tag->getOption< std::string >( "stationary" ) );

  name_ = tag->getOption< std::string >( "name" );

  rotation_mag_ = tag->getOption< core::Real >( "rotation_magnitude", 3.0 );
  translation_mag_ = tag->getOption< core::Real >( "translation_magnitude", 8.0 );
}

claims::EnvClaims UniformRigidBodyCM::yield_claims( core::pose::Pose const&,
                                                    basic::datacache::WriteableCacheableMapOP ){
  using core::Size;
  using core::pack::task::residue_selector::ResidueSubset;
  claims::EnvClaims claim_list;

  claims::JumpClaimOP jclaim = new claims::JumpClaim( this,
                                                      name() + DOCKJUMP_TAG,
                                                      mobile_label_,
                                                      stationary_label_ );

  jclaim->strength( claims::MUST_CONTROL, claims::CAN_CONTROL );

  claim_list.push_back( jclaim );

  return claim_list;
}

std::string UniformRigidBodyCM::get_name() const {
  return "UniformRigidBodyCM";
}

moves::MoverOP UniformRigidBodyCM::fresh_instance() const {
  return ClaimingMoverOP( new UniformRigidBodyCM() );
}

moves::MoverOP UniformRigidBodyCM::clone() const{
  return ClaimingMoverOP( new UniformRigidBodyCM( *this ) );
}

} // rigid
} // protocols
