// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/ScoreFunction.hh
/// @brief  Score function class
/// @author Phil Bradley
/// @author Christopher Miles (cmiles@uw.edu)

// Unit headers
#include <core/types.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>
#include <core/scoring/hbonds/HBondOptions.hh>
#include <core/scoring/mm/MMBondAngleResidueTypeParamSet.hh>

//Auto Headers
#include <core/chemical/ChemicalManager.fwd.hh>
#include <core/scoring/ScoringManager.fwd.hh>

// C/C++ Headers
#include <string>

using std::string;
using utility::vector1;

namespace core {
namespace scoring {
namespace methods {

EnergyMethodOptions::EnergyMethodOptions():
	// hard-wired default, but you can set this with etable_type( string )
	etable_type_(FA_STANDARD_DEFAULT),
	atom_vdw_atom_type_set_name_(chemical::CENTROID), // can be set, see below
	unfolded_energies_type_( UNFOLDED_SCORE12 ),
	exclude_protein_protein_hack_elec_(false), // rosetta++ defaulted to true!
	exclude_monomer_hack_elec_(false),
	exclude_DNA_DNA_(true), // rosetta++ default
	hbond_options_(new hbonds::HBondOptions()),
	cst_max_seq_sep_(core::SZ_MAX),
	bond_angle_residue_type_param_set_(NULL) {}
		

/// copy constructor
EnergyMethodOptions::EnergyMethodOptions(EnergyMethodOptions const & src)
	: ReferenceCount( src ) {
	*this = src;
}

EnergyMethodOptions::~EnergyMethodOptions() {}

/// copy operator
EnergyMethodOptions const &
EnergyMethodOptions::operator=(EnergyMethodOptions const & src) {
	etable_type_ = src.etable_type_;
	atom_vdw_atom_type_set_name_ = src.atom_vdw_atom_type_set_name_;
	unfolded_energies_type_ = src.unfolded_energies_type_;
	method_weights_ = src.method_weights_;
	ss_weights_ = src.ss_weights_;
	exclude_protein_protein_hack_elec_ = src.exclude_protein_protein_hack_elec_;
	exclude_monomer_hack_elec_ = src.exclude_monomer_hack_elec_;
	exclude_DNA_DNA_    = src.exclude_DNA_DNA_;
	hbond_options_ = new hbonds::HBondOptions( *(src.hbond_options_) );
	cst_max_seq_sep_ = src.cst_max_seq_sep_;
	bond_angle_central_atoms_to_score_ = src.bond_angle_central_atoms_to_score_;
	bond_angle_residue_type_param_set_ = src.bond_angle_residue_type_param_set_;
	return *this;
}

string const &
EnergyMethodOptions::etable_type() const {
	return etable_type_;
}

void
EnergyMethodOptions::etable_type(string const & type ) {
	etable_type_ = type;
}

string const &
EnergyMethodOptions::unfolded_energies_type() const {
	return unfolded_energies_type_;
}

void
EnergyMethodOptions::unfolded_energies_type(string const & type ) {
	unfolded_energies_type_ = type;
}

bool
EnergyMethodOptions::exclude_protein_protein_hack_elec() const {
	return exclude_protein_protein_hack_elec_;
}

void
EnergyMethodOptions::exclude_protein_protein_hack_elec( bool const setting ) {
	exclude_protein_protein_hack_elec_ = setting;
}

bool
EnergyMethodOptions::exclude_monomer_hack_elec() const {
	return exclude_monomer_hack_elec_;
}

void
EnergyMethodOptions::exclude_monomer_hack_elec( bool const setting ) {
	exclude_monomer_hack_elec_ = setting;
}

bool
EnergyMethodOptions::exclude_DNA_DNA() const {
	runtime_assert( hbond_options_->exclude_DNA_DNA() == exclude_DNA_DNA_ );
	return exclude_DNA_DNA_;
}

void
EnergyMethodOptions::exclude_DNA_DNA( bool const setting ) {
	exclude_DNA_DNA_ = setting;
	hbond_options_->exclude_DNA_DNA( setting );
}

hbonds::HBondOptions const &
EnergyMethodOptions::hbond_options() const {
	return *hbond_options_;
}

hbonds::HBondOptions &
EnergyMethodOptions::hbond_options() {
	return *hbond_options_;
}

void
EnergyMethodOptions::hbond_options( hbonds::HBondOptions const & opts ) {
	hbond_options_ = new hbonds::HBondOptions( opts );
}

/// @brief  This is used in the construction of the VDW_Energy's AtomVDW object
string const &
EnergyMethodOptions::atom_vdw_atom_type_set_name() const {
	return atom_vdw_atom_type_set_name_;
}

void
EnergyMethodOptions::atom_vdw_atom_type_set_name(string const & setting ) {
	atom_vdw_atom_type_set_name_ = setting;
}

void
EnergyMethodOptions::set_strand_strand_weights(
	int ss_lowstrand,
	int ss_cutoff
) {
	ss_weights_.set_ss_cutoff(ss_cutoff);
	ss_weights_.set_ss_lowstrand(ss_lowstrand);
}

SecondaryStructureWeights const &
EnergyMethodOptions::secondary_structure_weights() const {
	return ss_weights_;
}

SecondaryStructureWeights &
EnergyMethodOptions::secondary_structure_weights() {
	return ss_weights_;
}

bool
EnergyMethodOptions::has_method_weights( ScoreType const & type ) const {
	return ( method_weights_.find( type ) != method_weights_.end() );
}

vector1< Real > const &
EnergyMethodOptions::method_weights( ScoreType const & type ) const {
	MethodWeights::const_iterator it( method_weights_.find( type ) );
	if ( it == method_weights_.end() ) {
		utility_exit_with_message( "EnergyMethodOptions::method_weights do not exist: " +
			name_from_score_type(type));
	}
	return it->second;
}

void
EnergyMethodOptions::set_method_weights(
	ScoreType const & type,
	vector1< Real > const & wts)
{
	method_weights_[ type ] = wts;
}

core::Size
EnergyMethodOptions::cst_max_seq_sep() const {
	return cst_max_seq_sep_;
}

void
EnergyMethodOptions::cst_max_seq_sep( Size const setting ) {
	cst_max_seq_sep_ = setting;
}

/// deprecated
vector1<string> const &
EnergyMethodOptions::bond_angle_central_atoms_to_score() const {
	if (bond_angle_residue_type_param_set_) {
		return bond_angle_residue_type_param_set_->central_atoms_to_score();
	}
	return bond_angle_central_atoms_to_score_;
}

/// deprecated
void
EnergyMethodOptions::bond_angle_central_atoms_to_score(vector1<string> const & atom_names) {
	bond_angle_central_atoms_to_score_ = atom_names;
	if (bond_angle_residue_type_param_set_) {
		bond_angle_residue_type_param_set_->central_atoms_to_score(atom_names);
	}
}

core::scoring::mm::MMBondAngleResidueTypeParamSetOP
EnergyMethodOptions::bond_angle_residue_type_param_set() {
	return bond_angle_residue_type_param_set_;
}

core::scoring::mm::MMBondAngleResidueTypeParamSetCOP
EnergyMethodOptions::bond_angle_residue_type_param_set() const {
	return bond_angle_residue_type_param_set_;
}

void
EnergyMethodOptions::bond_angle_residue_type_param_set(core::scoring::mm::MMBondAngleResidueTypeParamSetOP param_set) {
	bond_angle_residue_type_param_set_ = param_set;
}

/// used inside ScoreFunctionInfo::operator==
bool
operator==( EnergyMethodOptions const & a, EnergyMethodOptions const & b ) {
	return ( ( a.etable_type_ == b.etable_type_ ) &&
		( a.atom_vdw_atom_type_set_name_ == b.atom_vdw_atom_type_set_name_ ) &&
		( a.unfolded_energies_type_ == b.unfolded_energies_type_ ) &&
		( a.method_weights_ == b.method_weights_ ) &&
		( a.ss_weights_ == b.ss_weights_ ) &&
		( a.exclude_protein_protein_hack_elec_ == b.exclude_protein_protein_hack_elec_ ) &&
		( a.exclude_DNA_DNA_ == b.exclude_DNA_DNA_ ) &&
		( * (a.hbond_options_) == * (b.hbond_options_) ) &&
		( a.cst_max_seq_sep_ == b.cst_max_seq_sep_ ) &&
		( a.bond_angle_central_atoms_to_score_ == b.bond_angle_central_atoms_to_score_ ) &&
		( a.bond_angle_residue_type_param_set_ == b.bond_angle_residue_type_param_set_ ) );
}

/// used inside ScoreFunctionInfo::operator==
bool
operator!=( EnergyMethodOptions const & a, EnergyMethodOptions const & b ) {
	return !( a == b );
}

void
EnergyMethodOptions::show( std::ostream & out ) const {
	if ( etable_type_.size() ) out << "EnergyMethodOptions::show: etable_type: " << etable_type_ <<'\n';
	for ( MethodWeights::const_iterator it=method_weights_.begin(), ite = method_weights_.end(); it != ite; ++it ) {
		out << "EnergyMethodOptions::show: method_weights: " << it->first;
		for ( Size i=1; i<= it->second.size(); ++i ) {
			out << ' ' << it->second[i];
		}
		out << '\n';
	}
	out << "EnergyMethodOptions::show: unfolded_energies_type: " << unfolded_energies_type_ << std::endl;
	out << "EnergyMethodOptions::show: atom_vdw_atom_type_set_name: " << atom_vdw_atom_type_set_name_ << std::endl;
	out << "EnergyMethodOptions::show: exclude_protein_protein_hack_elec: "
			<< (exclude_protein_protein_hack_elec_ ? "true" : "false") << std::endl;
	out << "EnergyMethodOptions::show: exclude_DNA_DNA: "
			<< (exclude_DNA_DNA_ ? "true" : "false") << std::endl;
	out << "EnergyMethodOptions::show: cst_max_seq_sep: " << cst_max_seq_sep_ << std::endl;
	out << "EnergyMethodOptions::show: bond_angle_central_atoms_to_score:";
	if (bond_angle_residue_type_param_set_) {
		out << "setting ignored";
	} else {
		for ( Size i=1; i <= bond_angle_central_atoms_to_score_.size(); ++i ) {
			out << " \"" << bond_angle_central_atoms_to_score_[i] << "\"";
		}
	}
	out << std::endl;
	out << "EnergyMethodOptions::show: bond_angle_residue_type_param_set: "
			<< (bond_angle_residue_type_param_set_ ? "in use" : "none") << std::endl;
	if (bond_angle_residue_type_param_set_) {
		out << "  central_atoms_to_score:";
		if (!bond_angle_residue_type_param_set_->central_atoms_to_score().size()) out << "all";
		for ( Size i=1; i <= bond_angle_residue_type_param_set_->central_atoms_to_score().size(); ++i ) {
			out << " \"" << bond_angle_residue_type_param_set_->central_atoms_to_score()[i] << "\"";
		}
		out << std::endl;
		out << "  use_residue_type_theta0: "
				<< (bond_angle_residue_type_param_set_->use_residue_type_theta0() ? "true" : "false") << std::endl;
	}
	out << *hbond_options_;
}

std::ostream& operator<<(std::ostream & out, EnergyMethodOptions const & options) {
	options.show( out );
	return out;
}

}
}
}
