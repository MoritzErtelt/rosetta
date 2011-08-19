// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/scoring/trie/trie_vs_trie.hh
/// @brief
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

#ifndef INCLUDED_core_scoring_etable_etrie_TrieCountPairAll_hh
#define INCLUDED_core_scoring_etable_etrie_TrieCountPairAll_hh

// Unit Headers
#include <core/scoring/etable/etrie/TrieCountPairAll.fwd.hh>

// Package Headers
#include <core/scoring/trie/TrieCountPairBase.hh>
#include <core/scoring/trie/RotamerTrie.fwd.hh>

#include <core/scoring/etable/etrie/CountPairData_1_2.fwd.hh>
#include <core/scoring/etable/etrie/CountPairData_1_3.fwd.hh>
#include <core/scoring/etable/etrie/CountPairDataGeneric.fwd.hh>
//XRW_B_T1
//#include <core/scoring/etable/CoarseEtableEnergy.fwd.hh>
//XRW_E_T1
#include <core/scoring/methods/MMLJEnergyInter.fwd.hh>

#include <core/scoring/hbonds/HBondEnergy.fwd.hh>
#include <core/scoring/hbonds/hbtrie/HBCPData.fwd.hh>


// ObjexxFLC Headers
#include <ObjexxFCL/FArray2D.fwd.hh>

//Auto Headers
#include <core/types.hh>


namespace core {
namespace scoring {
namespace etable {
namespace etrie {

class TrieCountPairAll : public trie::TrieCountPairBase
{
private:
	Size const res1_cpdat_;
	Size const res2_cpdat_;

public:

	TrieCountPairAll( Size res1_cpdat_for_res2, Size res2_cpdat_for_res1 )
	:
		res1_cpdat_( res1_cpdat_for_res2 ),
		res2_cpdat_( res2_cpdat_for_res1 )
	{}

	virtual ~TrieCountPairAll();
	/// ------- USEFUL FUNCTIONS -------------///

	template < class CPDATA1, class CPDATA2  >
	bool operator () ( CPDATA1 const & at1dat, CPDATA2 const & at2dat, Real & /*weight*/, Size & path_dist )
	{
		path_dist = at1dat.conn_dist( res1_cpdat_ ) + at2dat.conn_dist( res2_cpdat_ ) + 1;
		return true;
	}

	static
	void
	print();

	///---------- TYPE RESOLUTION FUNCTIONS ----------///
	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::EtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

//XRW_B_T1
/*
	////////////////////////////////// CoarseEtableEnergy ///////////////////////////////////
	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);
	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);


	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_1 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_2 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairData_1_3 > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< EtableAtom, CountPairDataGeneric > const & trie2,
		etable::CoarseEtableEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

*/
//XRW_E_T1

	// HBONDS
	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hbonds::hbtrie::HBAtom, hbonds::hbtrie::HBCPData >  const & trie1,
		trie::RotamerTrie< hbonds::hbtrie::HBAtom, hbonds::hbtrie::HBCPData >  const & trie2,
		hbonds::HBondEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);


	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hbonds::hbtrie::HBAtom, hbonds::hbtrie::HBCPData >  const & trie1,
		trie::RotamerTrie< hbonds::hbtrie::HBAtom, hbonds::hbtrie::HBCPData >  const & trie2,
		hbonds::HBondEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

/// Hack Elec Energy

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);
	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_1 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_2 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairData_1_3 > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< hackelec::ElecAtom, CountPairDataGeneric > const & trie2,
		hackelec::HackElecEnergy const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);


	/////////////////////////// MMLJEnergyInter //////////////////////////////
	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_trie(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		ObjexxFCL::FArray2D< core::PackerEnergy > & pair_energy_table,
		ObjexxFCL::FArray2D< core::PackerEnergy > & temp_table);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_1 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_2 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairData_1_3 > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

	virtual
	void
	resolve_trie_vs_path(
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie1,
		trie::RotamerTrie< mm::mmtrie::MMEnergyTableAtom, CountPairDataGeneric > const & trie2,
		methods::MMLJEnergyInter const & sfxn,
		utility::vector1< core::PackerEnergy > & pair_energy_vector,
		utility::vector1< core::PackerEnergy > & temp_vector);

};


} // namespace etrie
} // namespace etable
} // namespace scoring
} // namespace core

#endif
