// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file NoesyModule.hh
/// @brief main hook-up for the automatic NOESY assignment module
/// @detailed
///	  handling of input-output options
///       provides class NoesyModule
///
/// @author Oliver Lange

#ifndef INCLUDED_protocols_noesy_assign_NoesyModule_HH
#define INCLUDED_protocols_noesy_assign_NoesyModule_HH


// Unit Header
#include <protocols/noesy_assign/NoesyModule.fwd.hh>

// Package Headers
#include <protocols/noesy_assign/CrossPeak.hh>
#include <protocols/noesy_assign/PeakFileFormat.fwd.hh>
#include <protocols/noesy_assign/PeakAssignmentResidueMap.hh>
#include <protocols/noesy_assign/PeakAssignmentParameters.hh>
//#include <devel/noesy_assign/ResonanceList.fwd.hh>

// Project Headers
#include <core/types.hh>

// Utility headers
#include <utility/exit.hh>
#include <utility/vector1.hh>
#include <utility/pointer/ReferenceCount.hh>

//// C++ headers
// #include <cstdlib>
// #include <string>
// #include <list>
// #include <map>

namespace protocols {
namespace noesy_assign {

class NoesyModule : public utility::pointer::ReferenceCount {

private:
 static bool options_registered_;

public:
  ///@brief register options
  static void register_options();

public:

  ///@brief constructor  -- initialize with fast-sequence
  NoesyModule( std::string const& fasta_sequence );

  ///@brief assign NOE data, use models provided by DecoyIterator for scoring and restraint exclusion, if cycle=0 read cycle from cmd-line
  template< class DecoyIterator >
  void assign( DecoyIterator const& begin, DecoyIterator const& end, core::Size cycle = 0 );

  ///@brief same as above, but decoy file will be determined from commandline and read directly
  void assign ( core::Size cycle = 0 );

  ///@brief after assignment --> create appropriate constraints
  void generate_constraint_files(
      core::pose::Pose const& pose,
      std::string const& cst_fa_file,
      std::string const& cst_centroid_file,
      core::Size min_seq_separation
  ) const;

  ///@brief write assignments into peak-file...
  void write_assignments( std::string file = "use_cmd_line" );

  ///@brief reset assignments...  -- call before re-assigning peaks
  void reset();

  ///@brief returns true if -noesy::in::resonances and -noesy::in::peaks are in cmd-line
  static bool cmdline_options_activated();


  ///@brief return (cross)peak-list (peak-file)
  CrossPeakList const& crosspeaks() const { return *crosspeaks_; }

  ///@brief return resonance assignments (prot-file)
  ResonanceList const& resonances() const { return *main_resonances_; }

private:

  ///@brief return all input files
  void read_input_files();

  //  bool skip_network_analysis_; //moved to PeakAssignmentParameters
  ///@brief private data, peak-list and master-resonances (sometimes different resonances for different peak-lists, thus the name)
  CrossPeakListOP crosspeaks_;
  ResonanceListOP main_resonances_;
};

}
}

#endif
