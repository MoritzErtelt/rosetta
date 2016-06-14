// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   binder/test/T00.basic.hpp
/// @brief  Binder self-test file. Basic functionality.
/// @author Sergey Lyskov

#ifndef _INCLUDED_T00_basic_hpp_
#define _INCLUDED_T00_basic_hpp_

int global_int;
long global_long;

unsigned int global_unsigned_int;
unsigned long global_unsigned_long;

float global_float;
double global_double;

void foo() {}

int foo_int(int)   { return 0; }
int foo_int(int &) { return 0; }
int foo_int_not_binded(int *) { return 0; }

float foo_float(float)   { return 0; }
float foo_float(float &) { return 0; }
float foo_float_not_binded(float *) { return 0; }

double foo_double(double)   { return 0; }
double foo_double(double &) { return 0; }
double foo_double_not_binded(double *) { return 0; }

using VoidP = void *;

VoidP foo_void_p(VoidP) { return nullptr; }
VoidP foo_void_p(VoidP const &) { return nullptr; }
VoidP foo_void_p(VoidP &) { return nullptr; }

#endif // _INCLUDED_T00_basic_hpp_