// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// (C) 199x-2009 Rosetta% Commons participating institutions and developers.
// For more information, see http://www.rosettacommons.org/.

/// @file    devel/metal_interface/MetalSiteResidue.hh
/// @brief   Stores data that describes a metal-coordinating residue.
/// @details The intended use is for a utility::vector1 of MetalSiteResidues to describe a multiple-residue metal site.  The atom id's make it convenient generalize the process of adding metalsite constraints.
/// @author Bryan Der

#ifndef INCLUDED_devel_metal_interface_MetalSiteResidue_HH
#define INCLUDED_devel_metal_interface_MetalSiteResidue_HH

//Headers
#include <devel/metal_interface/MetalSiteResidue.fwd.hh>
#include <core/id/AtomID.hh>
#include <numeric/xyzVector.hh>
#include <utility/pointer/ReferenceCount.hh>
#include <utility/vector1.hh>

typedef numeric::xyzVector<core::Real> point;

namespace devel{
namespace metal_interface{


class MetalSiteResidue : public utility::pointer::ReferenceCount {

public:

	MetalSiteResidue();

	virtual ~MetalSiteResidue();

	virtual core::Size get_seqpos();
	virtual void set_seqpos( core::Size seqpos );

	virtual point get_ligand_atom_xyz();
	virtual void set_ligand_atom_xyz( point ligand_atom_xyz );

	virtual std::string get_ligand_atom_name();
	virtual void set_ligand_atom_name( std::string ligand_atom_name );

	virtual core::id::AtomID get_ligand_atom_id(); //atom_id( atomno, residue )
	virtual void set_ligand_atom_id( core::id::AtomID ligand_atom_id );

	virtual core::id::AtomID get_pre_ligand_atom_id();
	virtual void set_pre_ligand_atom_id( core::id::AtomID pre_ligand_atom_id );

	virtual core::id::AtomID get_pre_pre_ligand_atom_id();
	virtual void set_pre_pre_ligand_atom_id( core::id::AtomID pre_pre_ligand_atom_id );

	virtual std::string get_resname();
	virtual void set_resname( std::string resname );

private:

	core::Size seqpos_;
	point ligand_atom_xyz_;
	std::string ligand_atom_name_;
	core::id::AtomID ligand_atom_id_; //atom_id( atomno, residue )
	core::id::AtomID pre_ligand_atom_id_;
	core::id::AtomID pre_pre_ligand_atom_id_;
	std::string resname_;


};//end MetalSiteResidue


}//namespace metal_interface
}//namespace devel

#endif // INCLUDED_devel_metal_interface_MetalSiteResidue_HH
