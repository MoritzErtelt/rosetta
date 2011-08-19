// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is made available under the Rosetta Commons license.
// See http://www.rosettacommons.org/license
// (C) 199x-2007 University of Washington
// (C) 199x-2007 University of California Santa Cruz
// (C) 199x-2007 University of California San Francisco
// (C) 199x-2007 Johns Hopkins University
// (C) 199x-2007 University of North Carolina, Chapel Hill
// (C) 199x-2007 Vanderbilt University

/// @file   core/cartesian/md.hh
/// @brief  Atom tree minimization functions
/// @author Phil Bradley


#ifndef INCLUDED_protocols_cartesian_md_hh
#define INCLUDED_protocols_cartesian_md_hh


// Package headers
#include <core/optimization/types.hh>
#include <core/optimization/MinimizerMap.hh>
#include <core/optimization/Multifunc.hh>
#include <core/optimization/DOF_Node.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>
#include <core/conformation/Residue.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/kinematics/MoveMap.hh>

// ObjexxFCL headers
#include <ObjexxFCL/FArray2D.fwd.hh>


namespace protocols {
namespace cartesian {

struct CartesianAtom{
 core::Size res;
 core::Size index;

 core::id::AtomID atom_id;
 core::Vector position;
 core::Vector velocity;
 core::Vector force;

 core::Vector old_position;
 core::Vector old_velocity;
 core::Vector old_force;

 double mass;
};

struct MD_Bond{
    core::id::AtomID atom_id_1;
    core::id::AtomID atom_id_2;
    int index1;
    int index2;
    float length;
};

struct MD_Angle{
    core::id::AtomID atom_id_1;
    core::id::AtomID atom_id_2;
    core::id::AtomID atom_id_3;
    int index1;
    int index2;
    int index3;
    float length;
    float angle;
};

struct MD_HarmonicDihedral{
    core::id::AtomID atom_id_1;
    core::id::AtomID atom_id_2;
    core::id::AtomID atom_id_3;
    core::id::AtomID atom_id_4;
    int index1;
    int index2;
    int index3;
    int index4;
    float angle;
};



class MolecularDynamics {
public:
    MolecularDynamics(
        core::pose::PoseOP & inputpose,
        core::scoring::ScoreFunction const & scorefxn
    );




private: //functions

    void createCartesianArray( );
    void setCartesianPositionsFromPose( );
    void setPosePositionsFromCartesian( );
    void zeroForces();
    int  findCartomAtom( const core::id::AtomID  &id1 );
    void getCartesianDerivatives( core::scoring::ScoreFunction const & scorefxn );


    void createBondList( );
    void createAngleList( );
    void createDihedralList( );
    MD_HarmonicDihedral createDihedral(
                const core::conformation::Residue &rsd,
                std::string name1,
                std::string name2,
                std::string name3,
                std::string name4
    );
    MD_HarmonicDihedral createDihedral(
                const core::conformation::Residue &rsd1,
                const core::conformation::Residue &rsd2,
                const core::conformation::Residue &rsd3,
                const core::conformation::Residue &rsd4,
                std::string name1,
                std::string name2,
                std::string name3,
                std::string name4
    );

    void setDihedralDerivatives( );




    void doBondDerivatives( float &totalepot );
    void doAngleDerivatives(float &totalepot );
    void doDihedralDerivatives( float &totalepot );



    void createCartesianDerivatives( core::pose::Pose & pose,
                                                                     core::scoring::ScoreFunction const & scorefxn );


    void setInitialSpeeds(double tgtTemp);

    void calcKineticEnergy(
        float &ekin,
        float &Temp
    );


    void applyForces_BeeMan(
        float &kin,
        float &temp);


    void applyForces_LangevinIntegration(
        double T,
        float &kin,
        float &temp);



    void applyForces_ConjugateGradient(
        int Step,
        float &current_energy,
        float &m_OldEnergy
    );

    void createCartesianDerivatives( core::scoring::ScoreFunction const & scorefxn );


public:

    void doMinimising( core::scoring::ScoreFunction const & scorefxn );

    void doMD(core::scoring::ScoreFunction const & scorefxn,
                         int Steps,
                         float startTemp,
                         float endTemp);

    void testCartesianDerivatives( core::scoring::ScoreFunction const & scorefxn );




private:  //data

    utility::vector1< CartesianAtom > cartom;
    utility::vector1< MD_Bond > bondlist;
    utility::vector1< MD_Angle > anglelist;
    utility::vector1< MD_HarmonicDihedral > dihedrallist;

    core::kinematics::MoveMap mm;
    core::optimization::MinimizerMap min_map;
    core::kinematics::DomainMap domain_map;

    core::pose::PoseOP pose;


};


} // namespace optimization
} // namespace core

#endif //INCLUDED_protocols_cartesian_md_hh
