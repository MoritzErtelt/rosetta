// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file SequenceProfile.hh
/// @brief class definition for a sequence profile that represents each
/// position in a sequence as a probability distribution over the allowed amino
/// acids at that position.
/// @author James Thompson

#ifndef INCLUDED_core_sequence_SequenceProfile_hh
#define INCLUDED_core_sequence_SequenceProfile_hh

// Unit headers
#include <core/sequence/SequenceProfile.fwd.hh>

// Project headers
#include <core/types.hh>
#include <core/sequence/Sequence.hh>

// Utility headers
#include <utility/file/FileName.fwd.hh>

// C++ headers
#include <utility/vector1.hh>

namespace core {
namespace sequence {

class SequenceProfile : public Sequence {
	typedef std::string string;
	typedef utility::file::FileName FileName;
public:

	/// @brief ctors
	SequenceProfile() : temp_( 1.0 ) {}
	SequenceProfile(
		FileName const & fn,
		core::Real temp = 1.0
	) : temp_( temp ),
			profile_from_pssm_file_(false)
	{
		read_from_file( fn, temp_ );
	}

	SequenceProfile(
		utility::vector1< utility::vector1< core::Real > > prof,
		std::string const & sequence,
		std::string const & id,
		Size start = 1
	) :
		Sequence( sequence, id, start ),
		temp_( 1.0 ),
		profile_from_pssm_file_(false)
	{
		profile( prof );
		assert( profile().size() == length() );
	}

	/// @brief copy ctor
	SequenceProfile( SequenceProfile const & src ):
		Sequence()
	{
		*this = src;
	}

	/// @brief assignment operator.
	SequenceProfile & operator = ( SequenceProfile const & rhs ) {
		if ( this == &rhs ) return *this;

		id      ( rhs.id() );
		start   ( rhs.start()    );
		gap_char( rhs.gap_char() );
		sequence( rhs.sequence() );

		profile ( rhs.profile() );
		alphabet( rhs.alphabet() );
		temp    ( rhs.temp() );

		return *this;
	}

	/// @brief dtor
	virtual ~SequenceProfile() {}

	/// @brief Returns an owning pointer to a new SequenceProfile object,
	/// with data that is a deep copy of the information in this object.
	virtual SequenceOP clone() const {
		SequenceOP new_seq_op( new SequenceProfile( *this ) );
		return new_seq_op;
	}

	/// @brief Read an profile matrix from the given filename using the NCBI
	/// PSSM format for a position-specific scoring matrix.
	virtual void read_from_file( FileName const & fn );
	virtual void read_from_file( FileName const & fn, core::Real temp );

	/// @brief Generate the profile matrix from a sequence and a given substitution matrix
	virtual void generate_from_sequence( Sequence const & seq, std::string matrix="BLOSUM62" );

	/// @brief Multiply all profile weights by factor
	void rescale(core::Real factor=1);

	void convert_profile_to_probs();

	/// @brief Read profile matrix from the given filename using the NNMAKE
	/// .checkpoint format.
	void read_from_checkpoint( FileName const & fn );

	/// @brief Returns the 2D vector1 of Real values representing this profile.
	utility::vector1< utility::vector1< Real > > const & profile() const;

	/// @brief Sets the 2D vector1 of Real values representing this profile.
	void profile(
		utility::vector1< utility::vector1< Real > > const & new_profile
	);

	/// @brief Inserts a character at the given position.
	virtual void insert_char( core::Size pos, char new_char );

	/// @brief Deletes the given position from the Sequence and shifts
	/// everything else back by one.
	virtual void delete_position( core::Size pos );

	virtual std::string type() const {
		return "sequence_profile";
	}

	/// @brief Returns the number of distinct values at each position in this
	/// profile.
	Size width() const;

	/// @brief Returns the vector1 of values at this position.
	//utility::vector1< Real >
	utility::vector1< Real > const &
	prof_row( Size pos ) const;

	/// @brief Returns true if the data in this profile came from a pssm file.
	/// Returns false if it came from a checkpoint file
	bool
	profile_from_pssm_file() const {
		return profile_from_pssm_file_; }

	/// @brief returns the temperature used in computing profile probabilities
	core::Real temp() const {
		return temp_;
	}

	/// @brief sets temperature used in computing profile probabilities
	void temp( core::Real new_temp ) {
		temp_ = new_temp;
	}

	/// @brief Return the alphabet used by this sequence profile. This is an
	/// N-dimensional vector1 where N is the width of the profile, and the ith
	/// entry of any row in the profile represents the probability of ith
	/// character at that row in the sequence.
	utility::vector1< std::string > alphabet() const {
		return alphabet_;
	}

	void alphabet( utility::vector1< std::string > new_alphabet ) {
		alphabet_ = new_alphabet;
	}

	/// @brief Print this SequenceProfile object to the given std::ostream.
	friend std::ostream & operator<<(
		std::ostream & out, const SequenceProfile & p
	);

	/// @brief converts a vector1 of arbitrary scores to values using Bolztmann
	/// averaging at a given kT. Scores should follow the convention that more positive -> better score.
	void scores_to_probs_(
		utility::vector1< core::Real > & scores,
		core::Real kT
	);

private:
	void check_internals_() const;
	utility::vector1< std::string > alphabet_;
	utility::vector1< utility::vector1< Real > > profile_;

	// temp used to convert arbitrary scores to/from probabilities
	core::Real temp_;
	//whether the profile came from a .pssm file as opposed to a checkpoint file
	bool profile_from_pssm_file_;

}; // class SequenceProfile

} // sequence
} // core

#endif
