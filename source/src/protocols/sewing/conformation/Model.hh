// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet;
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/sewing/sampling/SewGraph.hh
///
/// @brief A set of simple container classes used by the SewingProtocol. These classes are used during the Hashing of coordinates, as well
/// as during assembly of low-resolution assemblies.
///
/// @author Tim Jacobs

#ifndef INCLUDED_protocols_sewing_conformation_Model_hh
#define INCLUDED_protocols_sewing_conformation_Model_hh

//Unit headers
#include <protocols/sewing/conformation/Model.fwd.hh>
#include <utility/pointer/ReferenceCount.hh>

//Protocol headers
#include <core/pose/Pose.fwd.hh>
#include <core/id/AtomID.hh>
#include <core/id/AtomID_Map.hh>

//Utility headers
#include <utility/vector1.hh>
#include <utility/fixedsizearray1.hh>
#include <numeric/xyzVector.hh>

//C++ headers
#include <set>
#include <map>

namespace protocols {
namespace sewing  {

static int const CHIMERA_SEGMENT = 0; //For chimeric model segments generated by ContinuousAssembly
//static int const POSE_SEGMENT = -1; //For Model segments created from a pose
//static int const POSE_CHIMERA_SEGMENT = -3; //For chimeric model segments that contain pose segments

struct SewAtom {
	core::Size atomno_;
	numeric::xyzVector<float> coords_;
};

struct SewResidue {

	SewResidue():
		resnum_(0),
		residue_type_(""),
		num_neighbors_(0)
	{}

	core::Size resnum_;
	//std::string pdb_resnum_;
	std::string residue_type_;
	utility::vector1<core::Real> chi_angles_;
	utility::vector1<SewAtom> basis_atoms_;

	core::Size num_neighbors_;

	//Used during assembly to map two residues to the same position. This
	//is used to generate 'native' rotamers during assembly optimization
	utility::vector1<SewResidue> matched_residues_;
};

struct SewSegment{

	SewSegment():
		model_id_(0),
		segment_id_(0),
		dssp_('N'),
		hash_(true),
		chimera_(false)
	{}

	friend
	bool
	operator ==(SewSegment const & a, SewSegment const & b){
		return
			a.model_id_ == b.model_id_ &&
			a.segment_id_ == b.segment_id_;
	}

	friend
	bool
	operator <(SewSegment const & a, SewSegment const & b){
		if ( a.model_id_ < b.model_id_ ) return true;
		if ( a.model_id_ == b.model_id_ && a.segment_id_ < b.segment_id_ ) return true;
		return false;
	}

	//Retain model id in the Segment for use of this struct in
	//the Assembly class as well (which contains segments from
	//multiple Models)
	int model_id_;
	core::Size segment_id_;
	char dssp_;
	utility::vector1<SewResidue> residues_;

	//Should this segment be included during hashing/scoring. This
	//was implemented to allow for linker segments to be included
	//in models between adjacent segments without considering these
	//linkers in the hash table
	bool hash_;

	//Used by the assembly infrastructure to specify that this
	//model is a chimera of other segments
	bool chimera_;

	//Vector of parents (pairs of model/segment ids)
	//utility::vector1< std::pair<int, core::Size> > parent_segments_;
};//struct SewSegment{


template <class T>
class ModelIterator
{

public:

	ModelIterator():
		seg_it_(/* 0 */),
		seg_end_it_(/* 0 */),
		res_it_(/* 0 */),
		atom_it_(/* 0 */)
	{}

	ModelIterator(
		typename utility::vector1<T>::iterator seg_it,
		typename utility::vector1<T>::iterator seg_end_it,
		utility::vector1<SewResidue>::iterator res_it,
		utility::vector1<SewAtom>::iterator atom_it
	):
		seg_it_(seg_it),
		seg_end_it_(seg_end_it),
		res_it_(res_it),
		atom_it_(atom_it)
	{}

	void
	operator ++(){
		++atom_it_;
		if ( atom_it_ == res_it_->basis_atoms_.end() ) {
			++res_it_;
			if ( res_it_ == seg_it_->residues_.end() ) {
				++seg_it_;
				if ( seg_it_ == seg_end_it_ ) {
					return;//we are done
				}
				res_it_ = seg_it_->residues_.begin();
			}
			atom_it_ = res_it_->basis_atoms_.begin();
		}
	}

	friend
	bool
	operator ==(ModelIterator const & a, ModelIterator const & b){
		return a.atom_it_ == b.atom_it_;
	}

	friend
	bool
	operator !=(ModelIterator const & a, ModelIterator const & b){
		return a.atom_it_ != b.atom_it_;
	}

	// ModelIterator &
	// operator =(ModelIterator const & rhs){
	//
	//  if(this == &rhs){
	//   return *this;
	//  }
	//  seg_it_ = rhs.seg_it_;
	//  seg_end_it_ = rhs.seg_end_it_;
	//  res_it_ = rhs.res_it_;
	//  atom_it_ = rhs.atom_it_;
	//  return *this;
	// }

	typename utility::vector1<T>::iterator
	segment(){
		return seg_it_;
	}

	utility::vector1<SewResidue>::iterator
	residue(){
		return res_it_;
	}

	utility::vector1<SewAtom>::iterator
	atom(){
		return atom_it_;
	}

private:
	typename utility::vector1<T>::iterator seg_it_;
	typename utility::vector1<T>::iterator seg_end_it_;
	utility::vector1<SewResidue>::iterator res_it_;
	utility::vector1<SewAtom>::iterator atom_it_;
};


template <class T>
class ModelConstIterator
{

public:

	ModelConstIterator(
		typename utility::vector1<T>::const_iterator seg_it,
		typename utility::vector1<T>::const_iterator seg_end_it,
		utility::vector1<SewResidue>::const_iterator res_it,
		utility::vector1<SewAtom>::const_iterator atom_it
	):
		seg_it_(seg_it),
		seg_end_it_(seg_end_it),
		res_it_(res_it),
		atom_it_(atom_it)
	{}

	void
	operator ++(){
		++atom_it_;
		if ( atom_it_ == res_it_->basis_atoms_.end() ) {
			++res_it_;
			if ( res_it_ == seg_it_->residues_.end() ) {
				++seg_it_;
				if ( seg_it_ == seg_end_it_ ) {
					return;//we are done
				}
				res_it_ = seg_it_->residues_.begin();
			}
			atom_it_ = res_it_->basis_atoms_.begin();
		}
	}

	friend
	bool
	operator ==(ModelConstIterator const & a, ModelConstIterator const & b){
		return a.atom_it_ == b.atom_it_;
	}

	friend
	bool
	operator !=(ModelConstIterator const & a, ModelConstIterator const & b){
		return a.atom_it_ != b.atom_it_;
	}

	typename utility::vector1<T>::const_iterator
	segment(){
		return seg_it_;
	}

	utility::vector1<SewResidue>::const_iterator
	residue(){
		return res_it_;
	}

	utility::vector1<SewAtom>::const_iterator
	atom(){
		return atom_it_;
	}

private:
	typename utility::vector1<T>::const_iterator seg_it_;
	typename utility::vector1<T>::const_iterator seg_end_it_;
	utility::vector1<SewResidue>::const_iterator res_it_;
	utility::vector1<SewAtom>::const_iterator atom_it_;
};


/// @details Simple wrapper around a vector1 of SewSegments, which
///maintains connection information.
class SegmentGraph : public utility::vector1<SewSegment> {

public:

	void
	add_connection(
		core::Size i,
		core::Size j
	);

	core::Size
	c_segment(
		core::Size ind
	) const;

	core::Size
	n_segment(
		core::Size ind
	) const;

	bool
	has_next(
		core::Size ind
	) const;

	void
	insert_connected_segment(
		SewSegment segment,
		core::Size ind
	);

	std::set<SewSegment>
	erase_segments(
		std::set<SewSegment> segments_to_erase
	);

	core::Size
	next(
		core::Size ind
	) const;

	core::Size
	previous(
		core::Size ind
	) const;

	bool
	has_previous(
		core::Size ind
	) const;

	std::map<core::Size,core::Size>
	forward_connections() const;

	std::map<core::Size,core::Size>
	reverse_connections() const;

	void
	reorder(
		utility::vector1<core::Size> new_order
	);

	void
	clear_connections();

private:
	std::map<core::Size, core::Size> forward_connections_;
	std::map<core::Size, core::Size> reverse_connections_;
};




struct Model : public utility::pointer::ReferenceCount {

	Model():
		model_id_(0),
		pdb_code_(""),
		structure_id_(0),
		distance_(0.0),
		hoist_angle_degrees_(0.0),
		packing_angle_degrees_(0.0),
		meridian_angle_degrees_(0.0)
	{}

	/// @brief create a pose for this model from the database
	core::pose::Pose
	to_pose_from_db() const;

	/// @brief trim the given pose to include only model residues
	void
	trim_db_pose(
		core::pose::Pose & pose
	) const;

	/// @brief get the corresponding pose resnum from the
	core::Size
	pose_number(
		core::Size resnum
	) const;

	/// @brief return the set of segment ids for this model
	std::set<core::Size>
	segment_ids() const;

	friend
	bool
	operator < (Model const & a, Model const & b) {
		return a.model_id_ < b.model_id_;
	}

	SewResidue
	get_residue(
		core::Size resnum
	) const;


	ModelConstIterator<SewSegment>
	model_begin() const;

	ModelIterator<SewSegment>
	model_begin();

	ModelConstIterator<SewSegment>
	model_end() const;

	ModelIterator<SewSegment>
	model_end();

	int model_id_;
	std::string pdb_code_;
	core::Size structure_id_;
	SegmentGraph segments_;

	core::Real distance_;
	core::Real hoist_angle_degrees_;
	core::Real packing_angle_degrees_;
	core::Real meridian_angle_degrees_;
};

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       UTILITY FUNCTIONS             //////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//std::map< int, Model >
//get_helix_models_from_db();
//
//std::map< int, Model >
//get_discontinuous_models_from_db();

std::map< int, Model >
get_discontinuous_models_from_db();

std::map< int, Model >
get_strand_sew_models_from_db();

std::map< int, Model >
get_continuous_models_from_db(
	std::string hash_between);

std::map< int, Model >
get_5_ss_models_from_db(
	std::string hash_between);

std::map< int, Model >
get_alpha_beta_models_from_db();

void
add_num_neighbors(
	std::map< int, Model > & models
);

void
add_linker_segments(
	std::map< int, Model > & models
);

void
create_alpha_beta_models_table();

/// @brief create a Model from the pose
Model
create_model_from_pose(
	core::pose::Pose const & pose,
	utility::vector1< std::pair<core::Size,core::Size> > const & segments,
	int model_id
);

void
remove_models_from_dssp(
	std::map< int, Model > & models,
	char dssp1,
	char dssp2
);

void
write_model_file(
	std::string const & comments,
	std::map< int, Model > models,
	std::string filename
);

std::map<int, Model>
read_model_file(
	std::string filename
);

} //sewing namespace
} //protocols namespace

#endif
