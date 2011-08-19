// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file core/io/silent/SilentStructFactory.hh
/// @brief Factory for creating various types of SilentStruct objects.
/// @author James Thompson

#ifndef INCLUDED_core_io_silent_SilentStructFactory_hh
#define INCLUDED_core_io_silent_SilentStructFactory_hh

#include <core/pose/Pose.fwd.hh>
#include <core/io/silent/SilentStruct.fwd.hh>
#include <core/io/silent/SilentStructCreator.fwd.hh>

#include <utility/vector1.hh>
#include <utility/factory/WidgetRegistrator.hh>

#include <map>

namespace core {
namespace io {
namespace silent {

class SilentStructFactory {
private:
	SilentStructFactory();

	SilentStructFactory(SilentStructFactory const &); // unimplemented
	SilentStructFactory const & operator=( SilentStructFactory const & ); // unimplemented
public:
	static SilentStructFactory * get_instance();

	void factory_register( SilentStructCreatorCOP creator );
	io::silent::SilentStructOP get_silent_struct( std::string const & type_name );
	utility::vector1< std::string > get_ss_names() const;

	/// @brief Replace the load-time SilentStructCreator with another creator.
	void replace_creator( SilentStructCreatorCOP creator );

	SilentStructCreatorCOP
	get_creator( std::string const & type_name );

	SilentStructOP get_silent_struct_in();
	SilentStructOP get_silent_struct_out();
	SilentStructOP get_silent_struct_out( core::pose::Pose const & pose );

private:

	static SilentStructFactory * instance_;

	typedef std::map< std::string, io::silent::SilentStructCreatorCOP > SilentStructCreatorMap;
	SilentStructCreatorMap ss_types_;
};

/// @brief This templated class will register an instance of an
/// SilentStructCreator (class T) with the SilentStructFactory.  It will ensure
/// that no SilentStructCreator is registered twice, and, centralizes
/// this registration logic so that thread safety issues can be handled in
/// one place
template < class T >
class SilentStructRegistrator : public utility::factory::WidgetRegistrator< SilentStructFactory, T >
{
public:
	typedef utility::factory::WidgetRegistrator< SilentStructFactory, T > parent;
public:
	SilentStructRegistrator() : parent() {}
};


} //silent
} //io
} //core

#endif
