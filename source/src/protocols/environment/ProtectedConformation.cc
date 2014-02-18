// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file src/protocols/moves/ProtectedConformation.cc
/// @author Justin Porter

// Unit Headers
#include <protocols/environment/ProtectedConformation.hh>

// Package headers
#include <core/environment/DofPassport.hh>
#include <protocols/environment/Environment.hh>
#include <protocols/environment/EnvExcn.hh>

// Project headers
#include <core/conformation/symmetry/SymmetricConformation.hh>
#include <core/kinematics/FoldTree.hh>
#include <core/id/TorsionID.hh>

#include <core/kinematics/AtomTree.hh>

// tracer
#include <basic/Tracer.hh>

// C++ Headers

// ObjexxFCL Headers

static basic::Tracer tr("protocols.environment.ProtectedConformation", basic::t_info);

namespace protocols {
namespace environment {

using core::environment::DofPassport;
using core::environment::DofPassportCOP;
using core::conformation::Conformation;
using core::environment::SequenceAnnotation;
using core::environment::SequenceAnnotationCOP;

std::string get_mover_name( std::stack< DofPassportCOP > const& unlocks ){
  if( unlocks.empty() ){
    return "UNKNOWN/NONE (no active passports)";
  } else {
    return unlocks.top()->mover();
  }
}

ProtectedConformation::ProtectedConformation( EnvironmentCAP env, Conformation const& src ):
  Conformation( src ),
  env_( env ),
  environment_exists_( true )
{
  environment()->pconf_creation( this );
}

ProtectedConformation::ProtectedConformation( ProtectedConformation const& src ):
  Conformation( src ),
  unlocks_( src.unlocks_ ),
  annotations_( src.annotations_ ),
  env_( src.environment() ),
  environment_exists_( true )
{
  environment()->pconf_creation( this );
}

ProtectedConformation::~ProtectedConformation() {
  if( environment_exists_ ){
    environment()->pconf_destruction( this );
  }
}

EnvironmentCAP ProtectedConformation::environment() const {
  if( environment_exists_ ){
    return env_;
  } else {
    throw utility::excn::EXCN_NullPointer( "ProtectedEnvironment asked for Environment pointer after environment has already been destroyed/gone out of scope." );
  }
}

core::conformation::ConformationOP ProtectedConformation::clone() const {
  return new ProtectedConformation( *this );
}

/////////////////////////////////////////////////////////////////////////////
//// SECURITY OVERLOADS 	///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Conformation& ProtectedConformation::operator=( Conformation const& src ){
  ProtectedConformationCOP conf = dynamic_cast< ProtectedConformation const* >( &src );

  unlocks_ = conf->unlocks_;
  annotations_ = conf->annotations_;
  env_ = conf->environment();
  return Parent::operator=( src );
}

void ProtectedConformation::set_torsion( core::id::TorsionID const & id, core::Real const setting ){
  if( verify( id ) ){
    Parent::set_torsion( id, setting );
  } else {
    fail_verification( utility::to_string(id) );
  }
}

void ProtectedConformation::set_jump( int const jump_number, core::kinematics::Jump const& new_jump ){
  set_jump( jump_atom_id( jump_number ), new_jump );
}

void ProtectedConformation::set_jump( core::id::AtomID const& id, core::kinematics::Jump const& new_jump ){
  if( verify_jump( id ) ){
    Parent::set_jump( id, new_jump );
  } else {
    fail_verification( utility::to_string(id) );
  }
}

void ProtectedConformation::set_secstruct( Size const seqpos, char const setting ){
  if( verify_backbone( seqpos ) ){
    return Parent::set_secstruct( seqpos, setting );
  }
  fail_verification( "secondary structure assignment" );
}

void ProtectedConformation::replace_residue( Size const seqpos, core::conformation::Residue const& new_rsd,
                                             utility::vector1< std::pair< std::string, std::string > > const & atom_pairs ){
  if( verify_backbone( seqpos ) ) {
    return Parent::replace_residue( seqpos, new_rsd, atom_pairs );
  }
  fail_verification( "residue replacement of residue "+utility::to_string( seqpos ) );
}

void ProtectedConformation::set_stub_transform( core::id::StubID const& stub_id1,
                                                core::id::StubID const& stub_id2,
                                                core::kinematics::RT const& target_rt ){
  int direction = 0;
  if( verify_jump( atom_tree().get_jump_atom_id( stub_id1, stub_id2, direction ) ) ){
    return Parent::set_stub_transform( stub_id1, stub_id2, target_rt );
  }
  fail_verification( "Stubs (AtomID="+utility::to_string( atom_tree().get_jump_atom_id( stub_id1, stub_id2, direction ) )
                     +", Stub1="+utility::to_string(stub_id1)+", Stub2="+utility::to_string(stub_id2) +")" );
}

void ProtectedConformation::set_dof( DOF_ID const& id, core::Real const setting ){
  if( verify( id ) ){
    return Parent::set_dof( id, setting);
  } else {
    fail_verification( utility::to_string( id ) );
  }
}

void ProtectedConformation::set_torsion_angle( AtomID const & atom1,
                                               AtomID const & atom2,
                                               AtomID const & atom3,
                                               AtomID const & atom4,
                                               core::Real const setting ){
  DOF_ID id = atom_tree().torsion_angle_dof_id( atom1, atom2, atom3, atom4 );
  if( verify( id ) ){
    return Parent::set_torsion_angle(atom1, atom2, atom3, atom4, setting);
  } else {
    fail_verification( utility::to_string(id)+" (via set_torsion_angle)" );
  }
}

void ProtectedConformation::set_bond_angle( AtomID const & atom1,
                                            AtomID const & atom2,
                                            AtomID const & atom3,
                                            core::Real const setting ){
  core::Real offset = 0.0;
  DOF_ID id = atom_tree().bond_angle_dof_id( atom1, atom2, atom3, offset );
  if( verify( id ) ){
    return Parent::set_bond_angle(atom1, atom2, atom3, setting);
  } else {
    fail_verification( utility::to_string(id)+" (via set_bond_angle)" );
  }
}

void ProtectedConformation::set_bond_length( AtomID const & atom1,
                                             AtomID const & atom2,
                                             core::Real const setting ){
  DOF_ID id = atom_tree().bond_length_dof_id( atom1, atom2 );
  if( verify( id ) ){
    return Parent::set_bond_length(atom1, atom2, setting);
  } else {
    fail_verification( utility::to_string(id)+" (via set_bond_angle)" );
  }
}

void ProtectedConformation::insert_fragment( core::id::StubID const & /* instub_id */,
		FragRT const & /* outstub_transforms */,
		FragXYZ const & /* frag_xyz */ )
{
	// TODO: implement
	runtime_assert( false );
}


// ANNOTATION-RELATED METHODS
void ProtectedConformation::attach_annotation( SequenceAnnotationCOP annotations ){
  annotations_ = annotations;
}

SequenceAnnotationCOP ProtectedConformation::resolver() const {
  return annotations_;
}

void ProtectedConformation::fork_annotation() {
  //TODO implmement annotation forking
  tr.Error << "fork_annotation is not fully implemented yet. It needs to actually make whatever changes are required to the annotation before it attaches it to the COP." << std::endl;
  runtime_assert( false );
  annotations_ = new SequenceAnnotation( annotations_ );
}


// MISC OVERRIDES
bool ProtectedConformation::same_type_as_me( Conformation const & other, bool recurse ) const {
  if( dynamic_cast< ProtectedConformation const* >( &other ) ){
    if( recurse ){
      return other.same_type_as_me( *this, false );
    } else {
      return true;
    }
  } else {
    return false;
  }
}


// Security housekeeping
void ProtectedConformation::set_environment( EnvironmentCAP env ){
  env_ = env;
}

void ProtectedConformation::push_passport( DofPassportCOP passport ){
  unlocks_.push( passport );
}

void ProtectedConformation::pop_passport(){
  unlocks_.pop();
}

bool ProtectedConformation::has_passport() const {
  return !unlocks_.empty();
}

// VERIFICATION CONVIENENCE METHODS

bool ProtectedConformation::verify_residue( Size const& seqpos ){
  if( verify_backbone( seqpos ) ){
    bool pass = true;
    for( Size i = 1; i <= residue( seqpos ).nchi(); ++i ){
      if( !verify( core::id::TorsionID( seqpos, core::id::CHI, i ) ) ){
        pass = false;
        break;
      }
    }
    return pass;
  }
  return false;
}

bool ProtectedConformation::verify_backbone( Size const& seqpos ){
  return ( verify( core::id::TorsionID( seqpos, core::id::BB, core::id::phi_torsion ) ) &&
           verify( core::id::TorsionID( seqpos, core::id::BB, core::id::psi_torsion ) ) &&
           verify( core::id::TorsionID( seqpos, core::id::BB, core::id::omega_torsion ) ) );
}

bool ProtectedConformation::verify_jump( core::id::AtomID const& a_id ){
  core::id::DOF_ID id = core::id::DOF_ID( a_id, core::id::RB1 );
  return verify( id );
}

bool ProtectedConformation::verify( core::id::TorsionID const& t_id ){
  core::id::DOF_ID id = Parent::dof_id_from_torsion_id( t_id );

  // The omega and psi just before, and phi just after, a cut are undefined--
  // this code checks for that and grants (meaningless) access.
  if( t_id.torsion() == core::id::phi_torsion ){
    if( Parent::fold_tree().is_cutpoint( (int) t_id.rsd() - 1 ) ||
        t_id.rsd() == 1 ){
      //allow free modification of undefined torisons
      return true;
    } else {
      return verify( id );
    }
  } else if( t_id.torsion() == core::id::omega_torsion ||
             t_id.torsion() == core::id::psi_torsion ) {
    if( Parent::fold_tree().is_cutpoint( (int) t_id.rsd() ) ||
        t_id.rsd() == Parent::size() ){
      //allow free modification of undefined torisons
      return true;
    } else {
      return verify( id );
    }
  } else {
    return verify( id );
  }
}

bool ProtectedConformation::verify( core::id::DOF_ID const& id ){
  if( !id.valid() ){
    throw utility::excn::EXCN_RangeError( "Invalid DOF_ID generated and used in ProtectedConformation." );
  }
  return ( !unlocks_.empty() && unlocks_.top()->dof_access( *environment(), id ) );
}

void ProtectedConformation::fail_verification( std::string const& str ){
  throw EXCN_Env_Security_Exception( str, *this, get_mover_name( unlocks_ ), environment() );
}

// ALWAYS-FAILING SECURITY OVERLOADS
void ProtectedConformation::contains_carbohydrate_residues( bool const ){
  fail_verification( "contains-carbohydrate flag" );
}

void ProtectedConformation::fold_tree( FoldTree const & ){
  fail_verification( "direct setting of foldtree" );
}

void ProtectedConformation::chain_endings( utility::vector1< Size > const & ) {
  fail_verification( "chain endings list" );
}

void ProtectedConformation::insert_chain_ending( Size const ) {
  fail_verification( "chain endings insertion" );
}

void ProtectedConformation::delete_chain_ending( Size const ) {
  fail_verification( "chain endings deletion" );
}

void ProtectedConformation::reset_chain_endings() {
  fail_verification( "chain endings reset" );
}

void ProtectedConformation::chains_from_termini() {
  fail_verification( "chain status from termini" );
}

void ProtectedConformation::append_residue_by_jump( core::conformation::Residue const&, Size const,
                                                   std::string const&, std::string const&,
                                                   bool const ) {
  fail_verification( "residue-by-jump append" );
}

void ProtectedConformation::append_polymer_residue_after_seqpos( core::conformation::Residue const&,
                                                                Size const, bool const ) {
  fail_verification( "residue-after-seqpos append" );
}

void ProtectedConformation::safely_append_polymer_residue_after_seqpos( core::conformation::Residue const&,
                                                                       Size const, bool const ) {
  fail_verification( "residue-after-seqpos append" );
}

void ProtectedConformation::prepend_polymer_residue_before_seqpos( core::conformation::Residue const&,
                                                                  Size const, bool const ) {
  fail_verification( "residue-before-seqpos prepend" );
}

void ProtectedConformation::safely_prepend_polymer_residue_before_seqpos( core::conformation::Residue const&,
                                                                         Size const, bool const ) {
  fail_verification( "residue-before-seqpos prepend" );
}

void ProtectedConformation::delete_polymer_residue( Size const ) {
  fail_verification( "polymer residue delete" );
}

void ProtectedConformation::delete_residue_slow( Size const ) {
  fail_verification( "slow delete residue" );
}

void ProtectedConformation::delete_residue_range_slow( Size const, Size const ) {
  fail_verification( "slow delete residue range" );
}

void ProtectedConformation::declare_chemical_bond( Size const, std::string const&, Size const,
                                                  std::string const& ) {
  fail_verification( "chemical bond declaration" );
}

void ProtectedConformation::insert_conformation_by_jump( Conformation const&, Size const, Size const,
                                                        Size const, Size const, std::string const&,
                                                        std::string const& ) {
  fail_verification( "conformation insertion (seriously?)" );
}

void ProtectedConformation::rebuild_polymer_bond_dependent_atoms( Size const ) {
  fail_verification( "polymer bond dependent atom rebuild"  );
}

void ProtectedConformation::insert_ideal_geometry_at_polymer_bond( Size const ) {
  fail_verification( "insert_ideal_geometry_at_polymer_bond" );
}

void ProtectedConformation::insert_ideal_geometry_at_residue_connection( Size const,
                                                                         Size const ) {
  fail_verification( "insert_ideal_geometry_at_residue_connection" );
}

void ProtectedConformation::set_polymeric_connection( Size, Size ){
  fail_verification( "set_polymeric_connection" );
}

void ProtectedConformation::fix_disulfides( utility::vector1< std::pair<Size, Size> > ){
  fail_verification( "fix_disulfides" );
}

void ProtectedConformation::set_xyz( AtomID const &, core::PointPosition const & )
{
	fail_verification( "set_xyz" );
}

void ProtectedConformation::batch_set_xyz( utility::vector1<AtomID> const &,
                                           utility::vector1< core::PointPosition > const & ){
  fail_verification( "batch_set_xyz" );
}

void ProtectedConformation::reset_move_data(){
  fail_verification( "reset_move_data" );
}

void ProtectedConformation::clear(){
  fail_verification( "clear" );
}

void ProtectedConformation::fill_missing_atoms( core::id::AtomID_Mask ){
  fail_verification( "fill_missing_atoms" );
}

} // environment
} // protocols
