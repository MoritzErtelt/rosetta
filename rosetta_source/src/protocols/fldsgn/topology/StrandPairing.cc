// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// This file is part of the Rosetta software suite and is made available under license.
// The Rosetta software is developed by the contributing members of the Rosetta Commons consortium.
// Copyright in the Rosetta software belongs to the developers and their institutions.
// For more information, see www.rosettacommons.org.

/// @file ./src/protocols/src/fldsgn/topology/StrandPairing.cc
/// @brief
/// @author Nobuyasu Koga ( nobuyasu@u.washington.edu )

// unit header
#include <protocols/fldsgn/topology/StrandPairing.hh>


// Project Headers
#include <protocols/fldsgn/topology/DimerPairing.hh>
#include <protocols/fldsgn/topology/SS_Info2.hh>

// utility headers
#include <utility/string_util.hh>
#include <utility/exit.hh>


// C++ headers
#include <cassert>
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <basic/Tracer.hh>


static basic::Tracer TR( "protocols.topology.StrandPairing" );

using namespace core;
typedef std::string String;
typedef utility::vector1< Size > VecSize;

namespace protocols {
namespace fldsgn {
namespace topology {

/// @brief default constructor
StrandPairing::StrandPairing():
	s1_( 0 ),
	s2_( 0 ),
	begin1_( 0 ),
	end1_( 0 ),
	begin2_( 0 ),
	end2_( 0 ),
	rgstr_shift_( 99 ),
	orient_( 'N' ),
	has_bulge_( false ),
	name_( "" )
{}


/// @brief value constructor
StrandPairing::StrandPairing(
  Size const s1,
	Size const s2,
	Size const b1,
	Size const b2,
	Size const p,
	Real const rs,
	char const o
):
	s1_( s1 ),
	s2_( s2 ),
	begin1_( b1 ),
	end1_( b1 ),
	begin2_( b2 ),
	end2_( b2 ),
	rgstr_shift_( rs ),
	orient_( o ),
	has_bulge_( false )
{
	runtime_assert( s1 < s2 );
	runtime_assert( b1 < b2 );
	pleats1_.push_back( p );
	pleats2_.push_back( p );
	residue_pair_.insert( std::map< Size, Size >::value_type( b1, b2 ) );
	residue_pair_.insert( std::map< Size, Size >::value_type( b2, b1 ) );
	initialize();
}


/// @brief value constructor
StrandPairing::StrandPairing(
  Size const s1,
	Size const s2,
	Real const rs,
	char const o
):
	s1_( s1 ),
	s2_( s2 ),
	begin1_( 0 ),
	end1_( 0 ),
	begin2_( 0 ),
	end2_( 0 ),
	rgstr_shift_( rs ),
	orient_( o ),
	has_bulge_( false )
{
	runtime_assert( s1 < s2 );
	initialize();
}


/// @brief value constructor
StrandPairing::StrandPairing( String const & spair ):
	begin1_( 0 ),
	end1_( 0 ),
	begin2_( 0 ),
	end2_( 0 ),
	has_bulge_( false )
{
	using utility::string_split;

	std::vector< String > parts( string_split( spair, '.' ) );
	runtime_assert( parts.size() == 3 );

	std::vector< String > st( string_split( parts[0], '-' ) );
	s1_ = boost::lexical_cast<Size>( st[0] );
	s2_ = boost::lexical_cast<Size>( st[1] );
	runtime_assert( s1_ < s2_ );

	orient_ = parts[1][0];
	runtime_assert( orient_ == 'P' || orient_ == 'A' || orient_ == 'N'  );
	rgstr_shift_ = boost::lexical_cast<Real>( parts[2] );

	name_ = spair;
}


/// @brief initialize StrandPairing
void
StrandPairing::initialize()
{
	using namespace boost;
	name_ = lexical_cast<String>(s1_) + '-' + lexical_cast<String>(s2_) + '.' + orient_ + '.'+ lexical_cast<String>(rgstr_shift_);
}


/// @brief copy constructor
StrandPairing::StrandPairing( StrandPairing const & sp ) :
	ReferenceCount(),
	s1_( sp.s1_ ),
	s2_( sp.s2_ ),
	begin1_( sp.begin1_ ),
	end1_( sp.end1_ ),
	begin2_( sp.begin2_ ),
	end2_( sp.end2_ ),
	pleats1_( sp.pleats1_ ),
	pleats2_( sp.pleats2_ ),
	rgstr_shift_( sp.rgstr_shift_ ),
	orient_( sp.orient_ ),
	has_bulge_( sp.has_bulge_ ),
	name_( sp.name_ ),
	residue_pair_( sp.residue_pair_ )
{}


/// @brief default destructor
StrandPairing::~StrandPairing(){}


/// @brief clone this object
StrandPairingOP
StrandPairing::clone()
{
	return new StrandPairing( *this );
}


/// @brief return name
std::ostream & operator<<( std::ostream & out, const StrandPairing &sp )
{
	out << sp.name();
	out << ": " << sp.begin1() << "-" << sp.end1() << "." << sp.begin2() << "-" << sp.end2();
	return out;
}


/// @brief
bool
StrandPairing::elongate( Size const r1, Size const r2, Size const p1, Size const p2 )
{
	runtime_assert( r2 > r1 );

	// set residue pair
	residue_pair_.insert( std::map< Size, Size >::value_type( r1, r2 ) );
	residue_pair_.insert( std::map< Size, Size >::value_type( r2, r1 ) );

	if ( begin1_ == 0 && begin2_ == 0 ) {
		begin1_ = r1;
		begin2_ = r2;
		end1_ = r1;
		end2_ = r2;
		pleats1_.push_back( p1 );
		pleats2_.push_back( p2 );
		return true;
	}

	if( r1 < end1_ ) {
		return false;
	}
	if( (orient_  ==  'P' && r2 < end2_) || (orient_ == 'A' && r2 > end2_) ) {
		return false;
	}

	// runtime_assert( r1 > end1_ );

	// 	if( orient_  ==  'P' ){
	// runtime_assert( r2 > end2_ );
	// }else{
	// runtime_assert( r2 < end2_ );
	// }

	end1_ = r1;
	end2_ = r2;

	if( size1() != size2() ){
		has_bulge_ = true;
	}

	return true;
}


/// @brief
bool
StrandPairing::add_pair( Size const r1, Size const r2, char const orient, Real const rgstr )
{
	runtime_assert( r2 > r1 );

	if( orient_ != orient ) return false;

	residue_pair_.insert( std::map< Size, Size >::value_type( r1, r2 ) );
	residue_pair_.insert( std::map< Size, Size >::value_type( r2, r1 ) );

	if ( begin1_ == 0 && begin2_ == 0 ) {
		begin1_ = r1;
		begin2_ = r2;
		end1_ = r1;
		end2_ = r2;
		return true;
	}

	if( r1 <= begin1_ ) {
		begin1_ = r1;
		rgstr_shift_ = rgstr;
	} else if( end1_ <= r1 ) {
		end1_ = r1;
	}

	if( r2 <= begin2_ ) {
		begin2_ = r2;
		rgstr_shift_ = rgstr;
	} else if( end2_ <= r2 ) {
		end2_ = r2;
	}

	if( size1() != size2() ){
		has_bulge_ = true;
	}
	return true;
}


/// @brief return length of 1st strand
Size
StrandPairing::size1() const
{
	return end1_ - begin1_ + 1;
}


/// @brief return length of 2nd strand
Size
StrandPairing::size2() const
{
	return end2_ - begin2_ + 1;
}


/// @brief return length of 2nd strand
bool
StrandPairing::is_parallel() const {
	if( orient_ == 'P' ) {
		return true;
	}else{
		return false;
	}
}

/// @brief whether input residue is included in this StrandPairinge or not
bool
StrandPairing::is_member( Size const res ) {

	if ( begin1_ <= end1_ ) {
		if( begin1_ <= res && res <= end1_ ) return true;
	} else {
		if( end1_ <= res && res <= begin1_ ) return true;
	}

	if ( begin2_ <= end2_ ) {
		if( begin2_ <= res && res <= end2_ ) return true;
	} else {
		if( end2_ <= res && res <= begin2_ ) return true;
	}

	return false;

}

/// @brief return residue pairing
Size
StrandPairing::residue_pair( Size const res )
{
	runtime_assert( begin1_ <= res && res <= end1_ || begin2_ <= res && res <= end2_ );
	return residue_pair_[ res ];
}

/// @brief reset begin1_, end1_, begin2_, end2_ based on ssinfo
void
StrandPairing::redefine_begin_end( SS_Info2_COP const ss_info )
{
	Real ir_ist = ss_info->strand( s1_ )->begin();
	Real len_ist = ss_info->strand( s1_ )->length();
	Real ir_jst = ss_info->strand( s2_ )->begin();
	Real er_jst = ss_info->strand( s2_ )->end();
	Real len_jst = ss_info->strand( s2_ )->length();

	Real start_ist, start_jst, len,inc;

	if( is_parallel() ){ // parallel

		inc = 1;

		if( rgstr_shift_ >= 0 ) {
			start_ist = ir_ist + rgstr_shift_;
			start_jst = ir_jst;
			if( len_ist >= (len_jst+rgstr_shift_) ) {
				//  i =========>
				//  j   =====>
				len = len_jst;
			}else{
				//  i =========>
				//  j   ==========>
				len = len_ist - rgstr_shift_;
			}
		} else {
			start_ist = ir_ist;
			start_jst = ir_jst - rgstr_shift_;
			if( len_ist >= (len_jst+rgstr_shift_) ) {
				//  i      ==========>
				//  j   ==========>
				len = len_jst + rgstr_shift_;
			} else {
				//  i      =====>
				//  j   ==========>
				len = len_ist ;
			}
		}

	} else { // anti parallel

		inc = -1;

		if( rgstr_shift_ >= 0 ) {
			start_ist = ir_ist + rgstr_shift_;
			start_jst = er_jst;
			if( len_ist >= (len_jst+rgstr_shift_) ) {
				//  i   ==========>
				//  j     <=====
				len = len_jst;
			} else {
				//  i   =========>
				//  j      <=========
				len = len_ist - rgstr_shift_;
			}

		} else {

			start_ist = ir_ist;
			start_jst = er_jst + rgstr_shift_;
			if( len_ist >= (len_jst+rgstr_shift_) ) {
				//  i     =========>
				//  j  <=========
				len = len_jst + rgstr_shift_;
			} else {
				//  i     =========>
				//  j  <==============
				len = len_ist;
			}

		} // if( rgstr_shift_ >= 0 )
	} // if is_parallel ?


	for( Size i=1; i<=len ;i++ ) {
		runtime_assert( start_ist > 0 && start_jst > 0 );
		if( ! elongate( Size( start_ist ), Size( start_jst ), 0, 0 ) ) {
			TR << "elongation failed ! " << std::endl;
			runtime_assert( false );
		}

		start_ist ++;
		start_jst += inc;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief default constructor
StrandPairingSet::StrandPairingSet():
	spairset_name_( "" ),
	num_strands_( 0 ),
	finalized_( false ),
	empty_( new StrandPairing )
{}


/// @brief value constructor
StrandPairingSet::StrandPairingSet( StrandPairings const & strand_pairings ):
	strand_pairings_( strand_pairings ),
	spairset_name_( "" ),
	num_strands_( 0 ),
	finalized_( false ),
	empty_( new StrandPairing )
{
	finalize();
}


/// @brief value constructor
StrandPairingSet::StrandPairingSet( String const & spairstring, SS_Info2_COP const ssinfo ):
	spairset_name_( "" ),
	num_strands_( 0 ),
	finalized_( false ),
	empty_( new StrandPairing )
{
	using utility::string_split;

	if( spairstring == "" ) {
		return;
	}

	std::vector< String > spairs( string_split( spairstring, ';' ) );
	for( std::vector< String >::const_iterator iter = spairs.begin(); iter != spairs.end() ; ++iter) {
		String spair( *iter );
		StrandPairingOP sp = new StrandPairing( spair );
		if( ssinfo ) {
			sp->redefine_begin_end( ssinfo );
		}
		push_back( sp );
	}
	finalize();
}


/// @brief value constructor
StrandPairingSet::StrandPairingSet( SS_Info2 const & ssinfo, DimerPairings const & dimer_pairs ):
	spairset_name_( "" ),
	num_strands_( 0 ),
	finalized_( false ),
	empty_( new StrandPairing )
{
	initialize_by_dimer_pairs( ssinfo, dimer_pairs );
}


/// @brief copy constructor
StrandPairingSet::StrandPairingSet( StrandPairingSet const & s ):
 	ReferenceCount(),
	strand_pairings_( s.strand_pairings_ ),
	spairset_name_( s.spairset_name_ ),
	num_strands_( s.num_strands_ ),
	finalized_( s.finalized_ ),
	map_strand_pairings_( s.map_strand_pairings_ ),
	neighbor_strands_( s.neighbor_strands_ ),
	empty_( s.empty_ )
{}


/// @brief destructor
StrandPairingSet::~StrandPairingSet(){}


/// @brief clone this object
StrandPairingSetOP
StrandPairingSet::clone() const
{
	return new StrandPairingSet( *this );
}


/// @brief output detail of strand pairing set
std::ostream & operator<<( std::ostream & out, const StrandPairingSet &s )
{
 	out << "#### StrandPairingSet Info " << std::endl;
	out << "# " << s.name() << std::endl;

	StrandPairings const & spairs( s.strand_pairings() );
	for( StrandPairings::const_iterator iter = spairs.begin(); iter != spairs.end(); ++iter ) {
		out << "# " << (**iter) << std::endl;
  }

	return out;
}


/// @brief add StrandPairingOP to StandPairingSet
void
StrandPairingSet::push_back( StrandPairingOP const sop )
{
	finalized_ = false;
	strand_pairings_.push_back( sop );
}


/// @brief add StrandPairingOP to StandPairingSet
void
StrandPairingSet::push_back_and_finalize( StrandPairingOP const sop )
{
	strand_pairings_.push_back( sop );
	finalize();
}


/// @brief clear data of this StrandParingSet
void
StrandPairingSet::clear()
{
	strand_pairings_.clear();
	spairset_name_  = "";
	finalized_ = false;
	map_strand_pairings_.clear();
	neighbor_strands_.clear();
}

/// @brief return num of strand_pairing
core::Size
StrandPairingSet::size() const
{
	return strand_pairings_.size();
}



/// @brief return spairset_name_
String
StrandPairingSet::name() const
{
	runtime_assert( finalized_ );
	return spairset_name_;
}


/// @brief return all strand pairings
StrandPairings const &
StrandPairingSet::strand_pairings() const
{
	return strand_pairings_;
}


/// @brief return one of the stand_pairings given a number
StrandPairingOP
StrandPairingSet::strand_pairing( Size const s ) const
{
	runtime_assert( s <= strand_pairings_.size() );
	return strand_pairings_[ s ];
}


/// @brief
StrandPairingOP
StrandPairingSet::strand_pairing( Size const s1, Size const s2 ) const
{
	runtime_assert( finalized_ );
	if( s1 <= num_strands_ && s2 <= num_strands_ ) {
		return map_strand_pairings_[ s1 ][ s2 ];
	} else {
		return empty_;
	}
}

/// @brief
StrandPairingSet::VecSize const &
StrandPairingSet::neighbor_strands( Size const s ) const
{
	runtime_assert( finalized_ );
	runtime_assert( s <= num_strands_ );
	return neighbor_strands_[ s ];
}


/// @brief the name of StrandPairingSet without register shift
/// For example, 2kl8 of ferredoxin-like fold is described as 1-3.A;2-3.A;1-4.A
String
StrandPairingSet::name_wo_rgstr() const
{
	String spairs = "";
	std::ostringstream name;
	for ( StrandPairings::const_iterator it=strand_pairings_.begin(),
						ite=strand_pairings_.end(); it != ite; ++it ) {
		StrandPairing const & spair( **it );
		std::vector< String > sp( utility::string_split( spair.name(), '.' ) );
		runtime_assert( sp.size() == 3 );
		if( spairs == "" ){
			spairs = sp[ 0 ] + '.' + sp[ 1 ];
		}else{
			spairs = spairs + ';' + sp[ 0 ] + '.' + sp[ 1 ];
		}
	}
	return spairs;
}


/// @brief
bool pointer_sorter( StrandPairingCOP const a, StrandPairingCOP const b )
{
	return ( a->name() < b->name() );
}


/// @brief
void
StrandPairingSet::finalize()
{
	typedef utility::vector1< Size > VecSize;

	finalized_ = true;

	// sort strand_parings_ by name_ of StrandPairingOP
	std::sort( strand_pairings_.begin(), strand_pairings_.end(), pointer_sorter );

	//
	for ( StrandPairings::const_iterator it=strand_pairings_.begin(),	ite=strand_pairings_.end(); it != ite; ++it ) {

		StrandPairing spair(**it);

		// set spairset_name_
		if( spairset_name_ == "" ){
			spairset_name_ = spair.name();
		}else{
			spairset_name_ += ';' + spair.name();
		}
		// find max number of strands
		if(	spair.s2() > num_strands_ ){
			num_strands_ = spair.s2();
		}
	}

	map_strand_pairings_.resize( num_strands_ );
	for( Size i=1; i<=num_strands_; i++ ) {

		// initialize neighbor_strands
		VecSize vec;
		neighbor_strands_.insert( std::map< Size, VecSize >::value_type( i, vec ) );

		// initialize map_strand_pairings_
		map_strand_pairings_[i].resize( num_strands_ );
		for( Size j=1; j<=num_strands_; j++ ) {
			map_strand_pairings_[i][j] = empty_;
		}
	}

	for ( StrandPairings::const_iterator it=strand_pairings_.begin(),	ite=strand_pairings_.end(); it != ite; ++it ) {
		StrandPairingOP const spair( *it );
		// define map_strand_pairings_
		map_strand_pairings_[ spair->s1() ][ spair->s2() ] = spair;
		map_strand_pairings_[ spair->s2() ][ spair->s1() ] = spair;
		// define neighbor_strands_
    VecSize & neighbor1 ( neighbor_strands_[ spair->s1() ] );
    VecSize & neighbor2 ( neighbor_strands_[ spair->s2() ] );
    neighbor1.push_back( spair->s2() );
    neighbor2.push_back( spair->s1() );
	}
	//TR << "#strands = " << num_strands_ << std::endl;
}


/// @brief
bool sort_by_length(
  protocols::fldsgn::topology::StrandPairingCOP const a,
	protocols::fldsgn::topology::StrandPairingCOP const b )
{
	return ( a->size1() >  b->size1() );
}

/// @brief
void
StrandPairingSet::drop_strand_pairs( StrandPairings const & drop_spairs )
{
	runtime_assert( drop_spairs.size() <= strand_pairings_.size() );

	StrandPairings new_spairs;
	for( Size jj=1; jj<=strand_pairings_.size(); jj++ ) {
		bool drop( false );
		for( Size ii=1; ii<=drop_spairs.size(); ii++ ) {
			if( strand_pairings_[ jj ]->name() == drop_spairs[ ii ]->name() ) {
				drop = true;
				break;
			}
		}

		if( ! drop ) {
			new_spairs.push_back( strand_pairings_[ jj ] );
		}
	}

	clear();
	strand_pairings_ = new_spairs;

	finalize();
}



/// @brief
void
StrandPairingSet::make_strand_neighbor_two()
{
	if( ! finalized_ ) {
		finalize();
	}

	utility::vector1< utility::vector1< bool > > pairmap( num_strands_, utility::vector1< bool >( num_strands_, false ) );

	bool modified( false );
	StrandPairings drop_spairs;
	for( Size ist=1; ist<=num_strands_; ist++ ) {

		StrandPairings spairs;
		if( neighbor_strands( ist ).size() > 2 ) {

			modified = true;
			for ( VecSize::const_iterator
							it=neighbor_strands( ist ).begin(), ite=neighbor_strands( ist ).end(); it != ite; ++it ) {
				Size jst( *it );
				spairs.push_back( strand_pairing( ist, jst ) );
			}
			std::sort( spairs.begin(), spairs.end(), sort_by_length );

			for( Size i=3; i<=spairs.size(); i++ ) {
				if( pairmap[ spairs[ i ]->s1() ][ spairs[ i ]->s2() ] ) continue;
				pairmap[ spairs[ i ]->s1() ][ spairs[ i ]->s2() ] = true;
				pairmap[ spairs[ i ]->s2() ][ spairs[ i ]->s1() ] = true;
				drop_spairs.push_back( spairs[ i ] );
			}
		}

	}

	if( modified ) {
		drop_strand_pairs( drop_spairs );
	}

}

/// @brief initialize StrandPairingSet based on dimer_pairs ( under developed )
void
StrandPairingSet::initialize_by_dimer_pairs( SS_Info2 const & ssinfo, DimerPairings const & dimer_pairs )
{
	// set number of strands
	num_strands_ = ssinfo.strands().size();

	// intialize map strand pairings
	map_strand_pairings_.resize( num_strands_ );
	for( Size i=1; i<=num_strands_; i++ ) {
		map_strand_pairings_[i].resize( num_strands_ );
		for( Size j=1; j<=num_strands_; j++ ) {
			map_strand_pairings_[i][j] = empty_;
		}
	}

	for ( DimerPairings::const_iterator it=dimer_pairs.begin(); it != dimer_pairs.end(); ++it ) {

		DimerPairing const & dp ( **it );

		if( (dp.sign1() == 1 && dp.sign2() == 1) || (dp.sign1() == 2 && dp.sign2() == 2) ) continue;

		Size iaa = dp.res1();
		Size jaa = dp.res2();
		Size istrand = ssinfo.strand_id( iaa );
		Size jstrand = ssinfo.strand_id( jaa );
		Size ist_begin = ssinfo.strand( istrand )->begin();
		Size jst_begin = ssinfo.strand( jstrand )->begin();
		Size jst_length = ssinfo.strand( jstrand )->length();

		StrandPairingOP & spop = map_strand_pairings_[ istrand ][ jstrand ];
		if( spop == 0 ){
			spop = new StrandPairing( istrand, jstrand, 0, dp.orient() );
			strand_pairings_.push_back( spop );
		}

		if( dp.orient() == 'A' ){

			Real rgstr = Real(iaa) - Real(ist_begin) - (Real(jst_length) - (Real(jaa) - Real(jst_begin)));
			spop->add_pair( iaa, jaa+1, dp.orient(), rgstr );
			spop->add_pair( iaa+1, jaa, dp.orient(), rgstr );

		} else if( dp.orient() == 'P' ) {

			Real rgstr = Real(iaa) - Real(ist_begin) - ( Real(jaa) - Real(jst_begin) );
			spop->add_pair( iaa, jaa, dp.orient(), rgstr );
			spop->add_pair( iaa+1, jaa+1, dp.orient(), rgstr );

		} else {
			runtime_assert( false );
		}

	}  // for ( DimerPairings )

	finalize();

}



} // namespace topology
} // namespace fldsgn
} // namespace protocols
