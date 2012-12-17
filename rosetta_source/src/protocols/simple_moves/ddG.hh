// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/protein_interface_design/movers/ddG.hh
/// @brief calculate interface ddg
/// @author Sarel Fleishman


#ifndef INCLUDED_protocols_simple_moves_ddG_hh
#define INCLUDED_protocols_simple_moves_ddG_hh

// Package headers
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/ScoreType.hh>
#include <basic/datacache/CacheableData.hh>
#include <core/scoring/methods/PoissonBoltzmannEnergy.hh>

#include <protocols/moves/Mover.fwd.hh>
#include <core/types.hh>
#include <protocols/simple_moves/ddG.fwd.hh>
#include <string>

#include <utility/vector1.hh>
#include <core/pack/task/TaskFactory.hh>

//Auto Headers
#include <protocols/simple_moves/DesignRepackMover.hh>

#include <vector>

namespace protocols {
namespace simple_moves {

class ddG : public simple_moves::DesignRepackMover
{
public:
	
	typedef core::Real Real;
	typedef core::scoring::ScoreType ScoreType;
	typedef core::pose::Pose Pose;

	/// Default step size used for translating unbounded pose
	static const Real DEFAULT_TRANS_STEP_SIZE;
	/// Default step size used for translating unbounded pose when Poisson-Boltzmann potential energy estimation is enabled.
	static const Real DEFAULT_TRANS_STEP_SIZE_PB;

public :
	ddG();
	ddG( core::scoring::ScoreFunctionCOP scorefxn_in, core::Size const jump=1, bool const symmetry=false );
	ddG( core::scoring::ScoreFunctionCOP scorefxn_in, core::Size const jump/*=1*/, utility::vector1<core::Size> const & chain_ids, bool const symmetry /*=false*/ );
	virtual void apply (Pose & pose);
	void calculate( Pose const & pose_in );
	void symm_ddG( core::pose::Pose & pose_in );
	void no_repack_ddG(core::pose::Pose & pose_in);
	void report_ddG( std::ostream & out ) const;
	Real sum_ddG() const;
	core::Size rb_jump() const { return rb_jump_; }
	void rb_jump( core::Size j ) { rb_jump_ = j; }
	core::Size repeats() const { return repeats_; }
	void repeats( core::Size repeats ) { repeats_ = repeats; }
	virtual ~ddG();
	protocols::moves::MoverOP fresh_instance() const { return (protocols::moves::MoverOP) new ddG; }
	protocols::moves::MoverOP clone() const;
	void parse_my_tag(  utility::tag::TagPtr const, protocols::moves::DataMap &, protocols::filters::Filters_map const &, protocols::moves::Movers_map const &, core::pose::Pose const& );

	virtual std::string get_name() const;
	protocols::moves::MoverOP relax_mover() const{ return relax_mover_; }
	void relax_mover( protocols::moves::MoverOP m ){ relax_mover_ = m; }

	void task_factory( core::pack::task::TaskFactoryOP task_factory ) { task_factory_ = task_factory; }
	core::pack::task::TaskFactoryOP task_factory() const { return task_factory_; }
	void use_custom_task( bool uct ) { use_custom_task_ = uct; }
	bool use_custom_task() const { return use_custom_task_; }
	void repack_bound( bool rpb ) { repack_bound_ = rpb; }
	bool repack_bound() const { return repack_bound_; }
	void relax_bound( bool rlb ) { relax_bound_ = rlb; }
	bool relax_bound() const { return relax_bound_; }

private :
	std::map< ScoreType, Real > bound_energies_;
	std::map< ScoreType, Real > unbound_energies_;

	std::map< Size, Real > bound_per_residue_energies_;
	std::map< Size, Real > unbound_per_residue_energies_;

	Real bound_total_energy_;
	Real unbound_total_energy_;
	Size repeats_;
	core::scoring::ScoreFunctionOP scorefxn_;
	void fill_energy_vector( Pose const & pose, std::map<ScoreType, Real > & energy_map );
	void fill_per_residue_energy_vector(Pose const & pose, std::map<Size,Real> & energy_map);
	core::Size rb_jump_;
	utility::vector1<core::Size> chain_ids_;
	bool symmetry_;
	bool per_residue_ddg_;
	bool repack_;
	protocols::moves::MoverOP relax_mover_; //dflt NULL; in the unbound state, relax before taking the energy

	core::pack::task::TaskFactoryOP task_factory_;
	bool use_custom_task_;
	bool repack_bound_;
	bool relax_bound_;

	/// info carrier for poisson-boltzmann potential energy computation
	core::scoring::methods::PBLifetimeCacheOP pb_cached_data_;
	/// step size for translating unbound pose.  the default size may be too large or too small for some applications.
	Real translation_step_size_;
};

} // movers
} // protocols
#endif /*INCLUDED_protocols_protein_interface_design_movers_ddG_HH*/

