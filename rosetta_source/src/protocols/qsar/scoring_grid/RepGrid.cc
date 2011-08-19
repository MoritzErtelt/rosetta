// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   /git/src/protocols/qsar/scoring_grid/AtrRepGrid.cc
/// @author Sam DeLuca

#include <protocols/qsar/scoring_grid/RepGrid.hh>
#include <protocols/qsar/scoring_grid/RepGridCreator.hh>
#include <protocols/qsar/qsarMap.fwd.hh>
#include <core/conformation/Residue.hh>
#include <utility/vector1.hh>
#include <basic/Tracer.hh>

namespace protocols {
namespace qsar {
namespace scoring_grid {

static basic::Tracer RepGridTracer("protocols.ligand_docking.scoring_grid.RepGrid");

std::string RepGridCreator::keyname() const
{
	return RepGridCreator::grid_name();
}

GridBaseOP RepGridCreator::create_grid() const
{
	return new RepGrid;
}

std::string RepGridCreator::grid_name()
{
	return "RepGrid";
}

RepGrid::RepGrid() : GridBase("RepGrid",0.0), radius_(2.25)
{

}

RepGrid::RepGrid(core::Real weight) : GridBase("RepGrid",weight), radius_(2.25)
{
}

void RepGrid::refresh( core::pose::Pose const & pose,  core::Vector const & )
{
	for(core::Size residue_index = 1; residue_index <= pose.total_residue(); ++residue_index)
	{
		//RepGridTracer <<"refreshing residue " <<residue_index << " of " << pose.total_residue() <<std::endl;
		core::conformation::Residue const & residue = pose.residue(residue_index);
		if(!residue.is_protein())
			continue;

		if(residue.has("CB"))
			this->set_sphere(residue.xyz("CB"),this->radius_,1.0);
		if(residue.has("N"))
			this->set_sphere(residue.xyz("N"),this->radius_,1.0);
		if(residue.has("CA"))
			this->set_sphere(residue.xyz("CA"),this->radius_,1.0);
		if(residue.has("C"))
			this->set_sphere(residue.xyz("C"),this->radius_,1.0);
		if(residue.has("O"))
			this->set_sphere(residue.xyz("O"),this->radius_,1.0);
	}
}

void RepGrid::refresh( core::pose::Pose const & pose,  core::Vector const & center, const core::Size & )
{
	//for the repulsive force, the case of no ligands and all ligands are identical
	refresh(pose, center);
}

void RepGrid::refresh(core::pose::Pose const & pose, core::Vector const & center,utility::vector1<core::Size> )
{
	//for the repulsive force, the case of no ligands and all ligands are identical
	refresh(pose,center);
}


}
}
}
