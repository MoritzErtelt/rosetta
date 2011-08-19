// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/pockets/PocketGrid.hh
/// @brief  protocols::pockets::PocketGrid header
/// @author David Johnson
/// @author Ragul Gowthaman

#ifndef INCLUDED_protocols_pockets_PocketGrid_hh
#define INCLUDED_protocols_pockets_PocketGrid_hh

#include <utility/pointer/ReferenceCount.hh>
#include <protocols/pockets/PocketGrid.fwd.hh>
#include <core/types.hh>
#include <core/pose/Pose.hh>
#include <core/conformation/Residue.fwd.hh>
#include <core/scoring/constraints/XYZ_Func.fwd.hh>

// Numeric Headers
#include <numeric/xyz.functions.hh>
#include <numeric/xyzMatrix.hh>
#include <numeric/xyzVector.hh>
#include <numeric/conversions.hh>

#include <utility/vector1_bool.hh>
#include <list>
#include <string>

namespace protocols {
namespace pockets {

///@
class PCluster
{

	friend class PocketGrid;
	friend class EggshellGrid;

public:

  PCluster(core::Size x, core::Size y, core::Size z, core::Real step_);
  PCluster(const PCluster& old);
  ~PCluster() {};
  int size() const {return points_.size();};
  bool testNeighbor(PCluster & c2);
  bool isClose(PCluster const & c2) const;
  bool isTarget() const {return target;};

private:
  typedef struct {
    core::Size x;
    core::Size y;
    core::Size z;
  } Cxyz;
  int count_;
    std::list < Cxyz > points_;
  bool target;
  core::Size maxX, minX, maxY, minY, maxZ, minZ;
  core::Real step;

}; //PCluster

///@
class PClusterSet
{

	friend class PocketGrid;
	friend class EggshellGrid;

public:
  PClusterSet();
  PClusterSet& operator= (const PClusterSet& old);
  void clear();
  void add (core::Size x, core::Size y, core::Size z, core::Real step);
  void findClusters();
  core::Real getLargestClusterSize( core::Real const & stepSize, core::Real const & minClusterSize );
  core::Real getNetClusterSize( core::Real const & stepSize, core::Real const & minClusterSize );
  core::Size size() { return clusters_.size(); }

private:
  std::list <PCluster> clusters_;

}; //PClusterSet

///@
class PocketGrid : public utility::pointer::ReferenceCount
{

	friend class EggshellGrid;

protected:
  enum PtType {EMPTY, PROTEIN, TARGET, HSURFACE, PSURFACE, POCKET, PO_SURF, PO_BURIED,T_SURFACE, TP_POCKET, TP_SURF, TP_BURIED, PO_EDGE, TP_EDGE,PSP, EGGSHELL, EGGSHELL_SURROUNDING };
	std::vector < std::vector < std::vector <PtType> > > grid_;
	std::vector < std::vector < std::vector <core::Size> > > pockets_;
  //std::vector < conformation::Atom > atoms_;
  //core::Size numAtoms_;
  core::Size xdim_, ydim_, zdim_;
  core::Real xcorn_;
  core::Real ycorn_;
  core::Real zcorn_;
  core::Real stepSize_;
  core::Real maxLen_;
  std::string tag_;
  void init();
  void setup_default_options();
  void newSearch(core::Size thr1, core::Size thr2, core::Size max1, core::Size max2, bool psp=false, bool sps=false);
  void searchX(core::Size thr, bool psp=false);
  void searchY(core::Size thr, bool psp=false);
  void searchZ(core::Size thr, bool psp=false);
  void searchD1(core::Size thr, bool psp=false);
  void searchD2(core::Size thr, bool psp=false);
  void searchD3(core::Size thr, bool psp=false);
  void searchD4(core::Size thr, bool psp=false);
  bool fill(core::Size x, core::Size y,core::Size z);
  bool touchesSolvent(core::Size x, core::Size y,core::Size z) const;
  bool touchesSS(core::Size x, core::Size y,core::Size z) const;
  bool touchesPS(core::Size x, core::Size y,core::Size z) const;
  bool touchesSurface(core::Size x, core::Size y,core::Size z, bool polar, bool either=false) const;
  numeric::xyzVector<core::Real> rotatePoint(core::Real x, core::Real y, core::Real z);

  core::Size pdbno_;
  PClusterSet clusters_;

  core::Real size_x_;
  core::Real size_y_;
  core::Real size_z_;
  core::Real spacing_;
  core::Real limit_x_;
  core::Real limit_y_;
  core::Real limit_z_;
  bool restrictSize_;
  core::Real probe_rad_;
  core::Real surf_score_;
  core::Real surf_dist_;
  core::Real bur_score_;
  core::Real bur_dist_;
  bool side_chains_only_;
  bool markpsp_;
  bool marksps_;
  core::Real minPockSize_;
  core::Real maxPockSize_;
  numeric::xyzMatrix<core::Real> rot_mat_;

public:
  PocketGrid();
  PocketGrid(const PocketGrid& gr);
  PocketGrid& operator= (const PocketGrid& gr);

  PocketGrid( core::conformation::Residue const & central_rsd );
  PocketGrid( std::vector< core::conformation::ResidueOP > const & central_rsd );

  PocketGrid( core::Real const & xc, core::Real const & yc, core::Real const & zc, core::Real x, core::Real y, core::Real z, core::Real const & stepSize=1, bool psp=false, bool sps=false);

  PocketGrid( core::Real const & xc, core::Real const & yc, core::Real const & zc, core::Real x, core::Real const & stepSize=1, bool psp=false, bool sps=false);
  PocketGrid( core::Vector const & center, core::Real x, core::Real const & stepSize=1, bool psp=false, bool sps=false);
  PocketGrid( core::Vector const & center, core::Real const & x, core::Real const & y, core::Real const & z, core::Real const & stepSize=1, bool psp=false, bool sps=false);

  ~PocketGrid() {};

  void initialize( core::Real const & xc, core::Real const & yc, core::Real const & zc, core::Real const size_x, core::Real const size_y, core::Real const size_z, core::Real const & stepSize=1, bool psp=false, bool sps=false);

  void initialize( core::Real const & xc, core::Real const & yc, core::Real const & zc, core::Real x, core::Real const & stepSize=1, bool psp=false, bool sps=false);
  void initialize( core::Vector const & center, core::Real x, core::Real const & stepSize=1, bool psp=false, bool sps=false);
  void initialize( core::Vector const & center, core::Real const & x, core::Real const & y, core::Real const & z, core::Real const & stepSize=1, bool psp=false, bool sps=false);
  void initialize( core::conformation::Residue const & central_rsd, core::Real const & x, core::Real const & y, core::Real const & z, core::Real const & stepSize=1, bool psp=false, bool sps=false);
  void initialize( std::vector< core::conformation::ResidueOP > const & central_rsd, core::Real const & x, core::Real const & y, core::Real const & z, core::Real const & stepSize=1, bool psp=false, bool sps=false);
  void clear();

  void recenter(core::Real const & xc, core::Real const & yc, core::Real const & zc);
	void recenter( core::conformation::Residue const & central_rsd );
	void recenter( std::vector< core::conformation::ResidueOP > const & central_rsds );
  void recenter( core::Vector const & center);

  void findPockets(core::Size thr, core::Real max);
  void findPSP(core::Size thr, core::Real max);
  void findSPS(core::Size thr, core::Real max);

  void mark (core::Real x, core::Real y, core::Real z, core::Real const & vdWd, core::Real const & buffer, bool polar=false, bool targetResi=false);
  void mark (core::Vector const & center, core::Real const & vdWd, core::Real const & buffer, bool polar=false, bool targetResi=false);

  void clearSmallPockets(core::Size minsize);
  void findClusters();

  void dumpGridToFile();
  void fillTargetPockets();
  void print();
  core::Real targetPocketVolume(core::Real const & surf_sc, core::Real const & bur_sc) const ;
  core::Real largestTargetPocketVolume();
  void markPocketDepth(core::Real const & surf_d, core::Real const & bur_d);
  void markEdgeDepth(core::Real const & surf_d, core::Real const & bur_d);
  core::Real netTargetPocketVolume();
  void markDepth(core::Size x, core::Size y, core::Size z, core::Real const & surf_d, core::Real const & bur_d);
  bool markOneEdgeDepth(core::Size x, core::Size y, core::Size z, core::Real const & surf_d, core::Real const & bur_d, bool isTarget);
  core::Real targetPocketSolventSurface() const ;
  core::Real targetPocketProteinSurface() const ;
  core::Real targetPocketHydrophobicProteinSurface() const ;
  core::Real targetPocketPolarProteinSurface() const ;
  core::Real targetPocketHeuristicScore() const ;
  bool isTooSmall() const;
  core::Vector whatIsTooSmall() const;

	bool autoexpanding_pocket_eval( core::conformation::Residue const & central_rsd, core::scoring::constraints::XYZ_Func const & xyz_func, Size const total_residues );
	bool autoexpanding_pocket_eval( std::vector< core::conformation::ResidueOP > const & central_rsd, core::scoring::constraints::XYZ_Func const & xyz_func, Size const total_residues );
	bool autoexpanding_pocket_eval( core::conformation::Residue const & central_rsd, core::pose::Pose const & inPose);
	bool autoexpanding_pocket_eval( std::vector< core::conformation::ResidueOP > const & central_rsd, core::pose::Pose const & inPose);

  static std::vector< core::conformation::ResidueOP > getRelaxResidues( core::pose::Pose const & input_pose, std::string const & resids );
  void randomAngle();
  void zeroAngle();

}; //class grid



///@
class EggshellGrid : public PocketGrid
{

	friend class NonPlaidFingerprint;

private:

	std::list< numeric::xyzVector<core::Real> > eggshell_coord_list_;
	std::list< numeric::xyzVector<core::Real> > extra_coord_list_;
	numeric::xyzVector<core::Real> pocket_CoM_;

public:

  EggshellGrid(const PocketGrid& gr);

	std::list< numeric::xyzVector<core::Real> > const & eggshell_coord_list() const { return eggshell_coord_list_; };
	std::list< numeric::xyzVector<core::Real> > const & extra_coord_list() const { return extra_coord_list_; };
	numeric::xyzVector<core::Real> pocket_CoM() const { return pocket_CoM_; };

}; //class grid



}//pockets
}//protocols

#endif
