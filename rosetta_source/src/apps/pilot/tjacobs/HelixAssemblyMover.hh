// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet;
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   HelixAssemblyMover.hh
///
/// @brief
/// @author Tim jacobs

#ifndef INCLUDED_apps_pilot_tjacobs_HelixAssemblyMover_hh
#define INCLUDED_apps_pilot_tjacobs_HelixAssemblyMover_hh

#include <core/pose/Pose.hh>
#include <protocols/moves/Mover.hh>

//mover definition
class HelixAssemblyMover : public protocols::moves::Mover {
public:

        ///@brief
        HelixAssemblyMover();

        virtual ~HelixAssemblyMover();

        virtual std::string get_name() const {
          return "HelixAssemblyMover";
        }

        std::string get_frag1_path() const;
        std::string get_frag2_path() const;
        core::Real get_helix_cap_distance_cutoff() const;
        core::Real get_helix_contact_distance_cutoff() const;
        core::Real get_helix_pair_rmsd_cutoff() const;
        core::Size get_minimum_helix_contacts() const;
        std::string get_query_structure_path() const;
        core::Real get_single_helix_rmsd_cutoff() const;
        void set_frag1_path(std::string frag1_path_);
        void set_frag2_path(std::string frag2_path_);
        void set_helix_cap_distance_cutoff(core::Real helix_cap_distance_cutoff_);
        void set_helix_contact_distance_cutoff(core::Real helix_contact_distance_cutoff_);
        void set_helix_pair_rmsd_cutoff(core::Real helix_pair_rmsd_cutoff_);
        void set_minimum_helix_contacts(core::Size minimum_helix_contacts_);
        void set_query_structure_path(std::string query_structure_path_);
        void set_single_helix_rmsd_cutoff(core::Real single_helix_rmsd_cutoff_);

        void init_from_options();

        core::pose::Pose combinePoses(const core::pose::Pose & pose1, const core::pose::Pose & pose2);

        utility::vector1<std::pair<core::Size,core::Size> > findHelices(const core::pose::Pose & pose);

        utility::vector1<core::Size> findFragments(const core::pose::Pose & targetPose, const core::pose::Pose & queryFragment, utility::vector1<std::pair<core::Size,core::Size> > helix_endpts);

        bool checkHelixContacts(const core::pose::Pose & pose, const core::pose::Pose & fragment1, const core::pose::Pose & fragment2, std::pair<core::Size,core::Size> helix_endpts);

        utility::vector1<std::pair<core::Size,core::Size> > findPartnerHelices(const core::pose::Pose & pose, const core::pose::Pose & fragment1, const core::pose::Pose & fragment2, core::Size frag1Start, core::Size frag2Start, utility::vector1<std::pair<core::Size,core::Size> > helix_endpts);

        void superimposeBundles(core::pose::Pose & pose1, const core::pose::Pose & pose2);

        virtual void apply(core::pose::Pose & pose);


private:

        std::string frag1_path_;
        std::string frag2_path_;
        std::string query_structure_path_;
        core::Real single_helix_rmsd_cutoff_;
        core::Real helix_pair_rmsd_cutoff_;
        core::Real helix_cap_distance_cutoff_;
        core::Real helix_contact_distance_cutoff_;
        core::Size minimum_helix_contacts_;

}; //end HelixAssemblyMover

#endif

