// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   numeric/random/WeightedSampler.hh
///
/// @brief  a class to generate random positive integers using a given set of weights
/// @author Colin A. Smith


#ifndef INCLUDED_numeric_random_WeightedSampler_hh
#define INCLUDED_numeric_random_WeightedSampler_hh


// numeric forward headers
#include <numeric/random/WeightedSampler.fwd.hh>


// numeric headers
#include <numeric/random/random.fwd.hh>
#include <numeric/types.hh>


// External library headers
#include <utility/vector1.hh>


// C++ headers


// Operating system headers


// Forward declarations




namespace numeric {
namespace random {


	/// @brief
class WeightedSampler
{

  // Friends


public: // Types


protected: // Types


private: // Types




public: // Constants


protected: // Constants


private: // Constants




public: // Creation


	/// @brief Constructor
	WeightedSampler();


	/// @brief Constructor
	WeightedSampler(
		numeric::Size num_weights
	);


	/// @brief Constructor
	WeightedSampler(
		utility::vector1<numeric::Real> const & weights
	);


	/// @brief Destructor
	virtual
	~WeightedSampler();


protected: // Creation


public: // Creation


	/// @brief Copy constructor
	WeightedSampler( WeightedSampler const & );




public: // Methods: assignment


	/// @brief Copy operator
	WeightedSampler &
	operator=( WeightedSampler const & );


public: // Methods: comparison




public: // Methods
	// Further subsections of methods allowed

	// get weights
	utility::vector1<numeric::Real> const &
	weights() const
	{
		return weights_;
	}

	// set weights
	void
	weights(
		utility::vector1<numeric::Real> const & weights
	)
	{
		weights_ = weights;
		cumulative_distribution_valid_ = false;
	}

	// add a single weight to the end
	void
	add_weight(
		numeric::Real weight
	)
	{
		weights_.push_back(weight);
		cumulative_distribution_valid_ = false;
	}

	// set a single weight
	void
	set_weight(
		numeric::Size weight_num,
		numeric::Real weight
	)
	{
		weights_[weight_num] = weight;
		cumulative_distribution_valid_ = false;
	}

	// clear weights
	void
	clear()
	{
		weights_.clear();
		cumulative_distribution_valid_ = false;
	}

	// get number of weights
	numeric::Size
	size() const
	{
		return weights_.size();
	}

	// resize weights
	void
	resize(
		numeric::Size num_weights,
		numeric::Real default_weight = 0
	)
	{
		weights_.resize(num_weights, default_weight);
	}

	// get a random sample by passing a random number from 0 to 1
	numeric::Size
	random_sample(
		numeric::Real randnum
	);

	// get a random sample by passing a random generator
	numeric::Size
	random_sample(
		numeric::random::RandomGenerator & rg
	);

protected: // Methods
	// Further subsections of methods allowed


private: // Methods
	// Further subsections of methods allowed

	void
	update_cumulative_distribution();


public: // Properties


protected: // Properties




private: // Fields

	utility::vector1<numeric::Real> weights_;
	utility::vector1<numeric::Real> cumulative_distribution_;
	bool cumulative_distribution_valid_;

}; // WeightedSampler


} // namespace random
} // namespace numeric


#endif // INCLUDED_numeric_random_WeightedSampler_HH
