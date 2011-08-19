// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file NoesyModule.hh
/// @author Oliver Lange

#ifndef INCLUDED_protocols_noesy_assign_NoesyModule_impl_hh
#define INCLUDED_protocols_noesy_assign_NoesyModule_impl_hh


// Unit Header
#include <protocols/noesy_assign/NoesyModule.hh>
#include <protocols/noesy_assign/CrossPeakList.impl.hh>
// Package Headers

// Project Headers
#include <core/types.hh>

// Utility headers
#include <utility/exit.hh>
#include <utility/vector1.hh>
#include <utility/pointer/ReferenceCount.hh>

#include <basic/prof.hh>

//// C++ headers
// #include <cstdlib>
// #include <string>
// #include <list>
// #include <map>

namespace protocols {
namespace noesy_assign {


template< class DecoyIterator >
void NoesyModule::assign( DecoyIterator const& decoys_begin, DecoyIterator const& decoys_end, core::Size cycle ) {
  using namespace core;
  if ( cycle != 0 ) PeakAssignmentParameters::set_cycle( cycle );
  //  using namespace basic::options;
  //  using namespace basic::options::OptionKeys::noesy;

  PROF_START( basic::NOESY_ASSIGN_INITIAL );
  crosspeaks_->find_assignments();
  PROF_STOP( basic::NOESY_ASSIGN_INITIAL );

  //  if ( !option[ no_remove_diagonal ]() )
  PROF_START( basic::NOESY_ASSIGN_DIAGONAL );
  crosspeaks_->delete_diagonal_peaks();
  PROF_STOP( basic::NOESY_ASSIGN_DIAGONAL );

  //if ( !option[ no_cs ]() )
  PROF_START( basic::NOESY_ASSIGN_CHEMSHIFT );
  crosspeaks_->update_chemshiftscore();
  PROF_STOP( basic::NOESY_ASSIGN_CHEMSHIFT );

  //  if ( !option[ no_symm ]() )
  PROF_START( basic::NOESY_ASSIGN_SYMMETRY );
  crosspeaks_->update_symmetry_score();
  PROF_STOP( basic::NOESY_ASSIGN_SYMMETRY );
  //  if ( !option[ no_upper ]() )

  PROF_START( basic::NOESY_ASSIGN_DISTANCE );
  crosspeaks_->update_upperdistance_score();
  PROF_STOP( basic::NOESY_ASSIGN_DISTANCE );

  PROF_START( basic::NOESY_ASSIGN_DECOY_COMP );
  if ( decoys_begin != decoys_end ) {
    crosspeaks_->update_decoy_compatibility_score( decoys_begin, decoys_end );
  } else {
    crosspeaks_->set_trivial_decoy_compatibility_score();
  }
  PROF_STOP( basic::NOESY_ASSIGN_DECOY_COMP );

  PROF_START( basic::NOESY_ASSIGN_NETWORK_TOTAL );
  PeakAssignmentParameters const& params( *PeakAssignmentParameters::get_instance() );
  if ( !params.no_network_ )
    crosspeaks_->network_analysis();
  crosspeaks_->update_peak_volumina();
  PROF_STOP( basic::NOESY_ASSIGN_NETWORK_TOTAL );


  //  if ( !option[  no_calibrate ] () )
  PROF_START( basic::NOESY_ASSIGN_CALIBRATE );
  crosspeaks_->calibrate( decoys_begin, decoys_end );
  PROF_STOP( basic::NOESY_ASSIGN_CALIBRATE );

  PROF_START( basic::NOESY_ASSIGN_ELIMINATE );
  crosspeaks_->eliminate_spurious_peaks();
  PROF_STOP( basic::NOESY_ASSIGN_ELIMINATE );
}

}
}

#endif
