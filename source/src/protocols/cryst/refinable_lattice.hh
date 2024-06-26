// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @author Frank DiMaio


#ifndef INCLUDED_protocols_cryst_refinable_lattice_hh
#define INCLUDED_protocols_cryst_refinable_lattice_hh

#include <protocols/cryst/spacegroup.hh>

#include <core/types.hh>

#include <core/pose/Pose.fwd.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/chemical/ChemicalManager.hh>
#include <core/chemical/ResidueTypeSet.hh>
#include <core/conformation/Residue.hh>
#include <core/conformation/ResidueFactory.hh>
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>



#include <protocols/moves/Mover.fwd.hh>


#include <utility/vector1.hh>
#include <numeric/xyzVector.hh>




#include <protocols/moves/Mover.hh> // AUTO IWYU For Mover
#include <core/conformation/symmetry/SymDof.hh> // AUTO IWYU For SymDof

namespace protocols {
namespace cryst {

class UpdateCrystInfo : public protocols::moves::Mover {
public:
	void apply( core::pose::Pose & pose ) override;


	protocols::moves::MoverOP clone() const override { return utility::pointer::make_shared< UpdateCrystInfo >(*this); }
	protocols::moves::MoverOP fresh_instance() const override { return utility::pointer::make_shared< UpdateCrystInfo >(); }

	void parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );
};


// calculate
class CrystRMS : public protocols::moves::Mover {
private:
	core::pose::PoseOP native_;
	core::scoring::ScoreFunctionOP sf_;

public:
	CrystRMS();

	void apply( core::pose::Pose & pose ) override;

	protocols::moves::MoverOP clone() const override { return utility::pointer::make_shared< CrystRMS >(*this); }
	protocols::moves::MoverOP fresh_instance() const override { return utility::pointer::make_shared< CrystRMS >(); }

	void parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );
};




class DockLatticeMover : public protocols::moves::Mover {
private:
	core::scoring::ScoreFunctionOP sf_;

	std::map< core::Size, core::conformation::symmetry::SymDof > symdofs_;
	core::Size SUBjump_,Ajump_,Bjump_,Cjump_;

	core::Real rot_mag_, trans_mag_, chi_mag_, kT_, contact_dist_;
	core::Real init_score_cut_;
	numeric::xyzVector< core::Real > lattice_mag_;
	core::Size ncycles_;

	std::string spacegroup_;
	core::Size mult_; // # copies in ASU
	bool randomize_;
	bool min_, min_lattice_, final_min_, pack_, perturb_chi_;

	bool verbose_;
	bool recover_low_;

	core::pose::PoseOP native_;

public:
	DockLatticeMover();
	DockLatticeMover(core::scoring::ScoreFunctionOP sf_cen_in);

	void
	set_defaults();

	// randomize, build initial lattice
	void
	initialize(core::pose::Pose & pose, core::Real contact_dist_scale=1.0);

	void
	randomize_chis( core::pose::Pose & pose );

	// perturbation movers
	void
	perturb_rb( core::pose::Pose & pose );
	void
	perturb_chis( core::pose::Pose & pose );
	void
	perturb_lattice( core::pose::Pose & pose );

	// repack
	void
	repack( core::pose::Pose & pose, bool rottrials=false );

	// slide into contact
	void
	slide_lattice( core::pose::Pose & pose );

	// regenerate symmetry
	// returns false if volume check failed
	bool
	regenerate_lattice( core::pose::Pose & pose );

	void apply( core::pose::Pose & pose ) override;

	void set_rot_mag(core::Real rot_mag_in) { rot_mag_=rot_mag_in;}
	void set_trans_mag(core::Real trans_mag_in) { trans_mag_=trans_mag_in;}
	void set_ncycles(core::Size ncycles_in) { ncycles_=ncycles_in;}
	void set_scorefunction(core::scoring::ScoreFunctionOP sf_new) { sf_=sf_new->clone(); }
	void set_temp(core::Real val) { kT_=val; }


	protocols::moves::MoverOP clone() const override { return utility::pointer::make_shared< DockLatticeMover >(*this); }
	protocols::moves::MoverOP fresh_instance() const override { return utility::pointer::make_shared< DockLatticeMover >(); }

	void parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


};


class MakeLatticeMover : public protocols::moves::Mover {
private:
	Spacegroup sg_;
	core::Real contact_dist_;
	bool refinable_lattice_, validate_lattice_;

	utility::vector1< numeric::xyzMatrix<core::Real> > allRs_;
	utility::vector1< numeric::xyzVector<core::Real> > allTs_;

public:
	MakeLatticeMover();

	void
	set_refinable_lattice( bool val ) {
		refinable_lattice_ = val;
	}

	void
	set_validate_lattice( bool val ) {
		validate_lattice_ = val;
	}

	// get R and T from subunit index
	void
	getRT( int idx, numeric::xyzMatrix<core::Real> &R, numeric::xyzVector<core::Real> &T ) {
		R = allRs_[idx];
		T = allTs_[idx];
	}

	// lowerlevel access to sg_
	utility::vector1<core::kinematics::RT> const &symmops() const { return sg_.symmops(); }
	Spacegroup const &sg() const { return sg_; }


	core::Size
	place_near_origin (
		core::pose::Pose & pose
	);

	void
	add_monomers_to_lattice(
		core::pose::Pose const & monomer_pose,
		core::pose::Pose & pose,
		utility::vector1<core::Size> const & monomer_anchors,
		utility::vector1<core::Size> & monomer_jumps,
		core::Size rootres
	);

	void
	detect_connecting_subunits(
		core::pose::Pose const & monomer_pose,
		core::pose::Pose const & pose,
		utility::vector1<core::Size> & monomer_anchors,
		core::Size &basesubunit
	);

	void
	build_lattice_of_virtuals(
		core::pose::Pose & pose,
		numeric::xyzVector<int> EXTEND,
		utility::vector1<core::Size> &Ajumps,
		utility::vector1<core::Size> &Bjumps,
		utility::vector1<core::Size> &Cjumps,
		utility::vector1<core::Size> &subunit_anchors,
		core::Size &basesubunit
	);

	void
	setup_xtal_symminfo(
		core::pose::Pose & pose,
		core::Size const num_monomers,
		core::Size const num_virtuals,
		core::Size const base_monomer,
		core::Size const nres_monomer,
		utility::vector1<core::Size> const &Ajumps,
		utility::vector1<core::Size> const &Bjumps,
		utility::vector1<core::Size> const &Cjumps,
		utility::vector1<core::Size> const &monomer_jumps,
		core::conformation::symmetry::SymmetryInfo & symminfo
	);


	//bool check_valid( core::pose::Pose & pose );
	void apply( core::pose::Pose & pose ) override;


	protocols::moves::MoverOP clone() const override { return utility::pointer::make_shared< MakeLatticeMover >(*this); }
	protocols::moves::MoverOP fresh_instance() const override { return utility::pointer::make_shared< MakeLatticeMover >(); }

	void
	spacegroup( Spacegroup const & sg_in ) {
		sg_ = sg_in;
	}

	void
	contact_dist( core::Real const setting ) {
		contact_dist_ = setting;
	}

	void parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


private:

	core::conformation::ResidueOP make_vrt( core::Vector O, core::Vector X, core::Vector Y, bool inv=false ) {
		core::conformation::ResidueOP vrtrsd;
		if ( inv ) {
			vrtrsd = core::conformation::ResidueOP( core::conformation::ResidueFactory::create_residue(
				core::chemical::ChemicalManager::get_instance()->residue_type_set(
				core::chemical::FA_STANDARD )->name_map( "INV_VRT" ) ) );
		} else {
			vrtrsd = core::conformation::ResidueOP( core::conformation::ResidueFactory::create_residue(
				core::chemical::ChemicalManager::get_instance()->residue_type_set(
				core::chemical::FA_STANDARD )->name_map( "VRT" ) ) );
		}
		vrtrsd->set_xyz("ORIG",O);
		vrtrsd->set_xyz("X",O+X);
		vrtrsd->set_xyz("Y",O+Y);
		return vrtrsd;
	}
};

}
}

#endif
