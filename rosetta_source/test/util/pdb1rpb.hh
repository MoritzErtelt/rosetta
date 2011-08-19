// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
// :noTabs=false:tabSize=4:indentSize=4:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   test/util/pdb1rpb.hh
/// @brief  create a 21 residue pose with two disulfides.
/// @author Andrew Leaver-Fay

#ifndef INCLUDED_util_pdb1rpb_HH
#define INCLUDED_util_pdb1rpb_HH

#include <test/util/pose_funcs.hh>

std::string pdb1rpb();

inline
core::pose::Pose
pdb1rpb_pose()
{
	return fullatom_pose_from_string( pdb1rpb() );
}

core::pose::PoseOP
pdb1rpb_poseop()
{
	return fullatom_poseop_from_string( pdb1rpb() );
}

inline
std::string
pdb1rpb()
{
	return
	"ATOM      1  N   CYS A   1       1.713   0.039   1.948  1.00  0.00           N  \n"
	"ATOM      2  CA  CYS A   1       0.498   0.600   1.280  1.00  0.00           C  \n"
	"ATOM      3  C   CYS A   1       0.204   0.148  -0.196  1.00  0.00           C  \n"
	"ATOM      4  O   CYS A   1       0.008   0.953  -1.111  1.00  0.00           O  \n"
	"ATOM      5  CB  CYS A   1       0.474   2.131   1.494  1.00  0.00           C  \n"
	"ATOM      6  SG  CYS A   1      -1.249   2.654   1.519  1.00  0.00           S  \n"
	"ATOM      7  H   CYS A   1       2.581  -0.153   1.436  1.00  0.00           H  \n"
	"ATOM      8  HA  CYS A   1      -0.379   0.192   1.816  1.00  0.00           H  \n"
	"ATOM      9  HB2 CYS A   1       0.921   2.426   2.463  1.00  0.00           H  \n"
	"ATOM     10  HB3 CYS A   1       1.044   2.664   0.711  1.00  0.00           H  \n"
	"ATOM     11  N   LEU A   2       0.067  -1.168  -0.397  1.00  0.00           N  \n"
	"ATOM     12  CA  LEU A   2      -0.464  -1.785  -1.656  1.00  0.00           C  \n"
	"ATOM     13  C   LEU A   2      -1.590  -2.850  -1.333  1.00  0.00           C  \n"
	"ATOM     14  O   LEU A   2      -1.984  -2.982  -0.173  1.00  0.00           O  \n"
	"ATOM     15  CB  LEU A   2       0.713  -2.145  -2.615  1.00  0.00           C  \n"
	"ATOM     16  CG  LEU A   2       1.327  -3.547  -2.476  1.00  0.00           C  \n"
	"ATOM     17  CD1 LEU A   2       2.439  -3.767  -3.507  1.00  0.00           C  \n"
	"ATOM     18  CD2 LEU A   2       1.869  -3.761  -1.064  1.00  0.00           C  \n"
	"ATOM     19  H   LEU A   2       0.293  -1.720   0.436  1.00  0.00           H  \n"
	"ATOM     20  HA  LEU A   2      -1.008  -1.033  -2.218  1.00  0.00           H  \n"
	"ATOM     21  HB2 LEU A   2       0.359  -2.030  -3.657  1.00  0.00           H  \n"
	"ATOM     22  HB3 LEU A   2       1.511  -1.375  -2.539  1.00  0.00           H  \n"
	"ATOM     23  HG  LEU A   2       0.511  -4.265  -2.666  1.00  0.00           H  \n"
	"ATOM     24 HD11 LEU A   2       2.068  -3.635  -4.541  1.00  0.00           H  \n"
	"ATOM     25 HD12 LEU A   2       3.277  -3.058  -3.367  1.00  0.00           H  \n"
	"ATOM     26 HD13 LEU A   2       2.856  -4.788  -3.443  1.00  0.00           H  \n"
	"ATOM     27 HD21 LEU A   2       2.517  -2.921  -0.744  1.00  0.00           H  \n"
	"ATOM     28 HD22 LEU A   2       1.029  -3.795  -0.345  1.00  0.00           H  \n"
	"ATOM     29 HD23 LEU A   2       2.428  -4.700  -0.954  1.00  0.00           H  \n"
	"ATOM     30  N   GLY A   3      -2.140  -3.607  -2.308  1.00  0.00           N  \n"
	"ATOM     31  CA  GLY A   3      -3.199  -4.641  -2.028  1.00  0.00           C  \n"
	"ATOM     32  C   GLY A   3      -2.835  -6.144  -2.112  1.00  0.00           C  \n"
	"ATOM     33  O   GLY A   3      -3.650  -6.937  -2.581  1.00  0.00           O  \n"
	"ATOM     34  H   GLY A   3      -1.774  -3.441  -3.249  1.00  0.00           H  \n"
	"ATOM     35  HA2 GLY A   3      -3.676  -4.490  -1.044  1.00  0.00           H  \n"
	"ATOM     36  HA3 GLY A   3      -4.053  -4.488  -2.704  1.00  0.00           H  \n"
	"ATOM     37  N   ILE A   4      -1.645  -6.537  -1.640  1.00  0.00           N  \n"
	"ATOM     38  CA  ILE A   4      -1.191  -7.966  -1.595  1.00  0.00           C  \n"
	"ATOM     39  C   ILE A   4      -1.794  -8.782  -0.380  1.00  0.00           C  \n"
	"ATOM     40  O   ILE A   4      -2.970  -8.602  -0.053  1.00  0.00           O  \n"
	"ATOM     41  CB  ILE A   4       0.379  -7.946  -1.817  1.00  0.00           C  \n"
	"ATOM     42  CG1 ILE A   4       1.068  -9.241  -2.328  1.00  0.00           C  \n"
	"ATOM     43  CG2 ILE A   4       1.231  -7.445  -0.618  1.00  0.00           C  \n"
	"ATOM     44  CD1 ILE A   4       0.338  -9.989  -3.455  1.00  0.00           C  \n"
	"ATOM     45  H   ILE A   4      -1.031  -5.745  -1.461  1.00  0.00           H  \n"
	"ATOM     46  HA  ILE A   4      -1.655  -8.447  -2.481  1.00  0.00           H  \n"
	"ATOM     47  HB  ILE A   4       0.553  -7.221  -2.641  1.00  0.00           H  \n"
	"ATOM     48 HG12 ILE A   4       2.039  -8.907  -2.731  1.00  0.00           H  \n"
	"ATOM     49 HG13 ILE A   4       1.350  -9.950  -1.514  1.00  0.00           H  \n"
	"ATOM     50 HG21 ILE A   4       0.847  -6.505  -0.190  1.00  0.00           H  \n"
	"ATOM     51 HG22 ILE A   4       1.289  -8.180   0.208  1.00  0.00           H  \n"
	"ATOM     52 HG23 ILE A   4       2.275  -7.244  -0.921  1.00  0.00           H  \n"
	"ATOM     53 HD11 ILE A   4       0.082  -9.317  -4.297  1.00  0.00           H  \n"
	"ATOM     54 HD12 ILE A   4       0.960 -10.805  -3.867  1.00  0.00           H  \n"
	"ATOM     55 HD13 ILE A   4      -0.604 -10.450  -3.103  1.00  0.00           H  \n"
	"ATOM     56  N   GLY A   5      -1.059  -9.700   0.283  1.00  0.00           N  \n"
	"ATOM     57  CA  GLY A   5      -1.633 -10.502   1.406  1.00  0.00           C  \n"
	"ATOM     58  C   GLY A   5      -0.698 -11.248   2.371  1.00  0.00           C  \n"
	"ATOM     59  O   GLY A   5      -0.818 -12.461   2.541  1.00  0.00           O  \n"
	"ATOM     60  H   GLY A   5      -0.119  -9.842  -0.097  1.00  0.00           H  \n"
	"ATOM     61  HA2 GLY A   5      -2.306  -9.880   2.028  1.00  0.00           H  \n"
	"ATOM     62  HA3 GLY A   5      -2.300 -11.262   0.983  1.00  0.00           H  \n"
	"ATOM     63  N   SER A   6       0.170 -10.502   3.053  1.00  0.00           N  \n"
	"ATOM     64  CA  SER A   6       1.020 -11.040   4.160  1.00  0.00           C  \n"
	"ATOM     65  C   SER A   6       0.895 -10.206   5.484  1.00  0.00           C  \n"
	"ATOM     66  O   SER A   6       0.420 -10.727   6.495  1.00  0.00           O  \n"
	"ATOM     67  CB  SER A   6       2.466 -11.227   3.638  1.00  0.00           C  \n"
	"ATOM     68  OG  SER A   6       3.303 -11.829   4.627  1.00  0.00           O  \n"
	"ATOM     69  H   SER A   6       0.172  -9.536   2.722  1.00  0.00           H  \n"
	"ATOM     70  HA  SER A   6       0.672 -12.055   4.444  1.00  0.00           H  \n"
	"ATOM     71  HB2 SER A   6       2.463 -11.872   2.736  1.00  0.00           H  \n"
	"ATOM     72  HB3 SER A   6       2.902 -10.266   3.314  1.00  0.00           H  \n"
	"ATOM     73  HG  SER A   6       3.247 -11.296   5.427  1.00  0.00           H  \n"
	"ATOM     74  N   CYS A   7       1.339  -8.936   5.502  1.00  0.00           N  \n"
	"ATOM     75  CA  CYS A   7       1.316  -8.057   6.710  1.00  0.00           C  \n"
	"ATOM     76  C   CYS A   7       0.898  -6.575   6.366  1.00  0.00           C  \n"
	"ATOM     77  O   CYS A   7       0.223  -6.348   5.362  1.00  0.00           O  \n"
	"ATOM     78  CB  CYS A   7       2.684  -8.286   7.410  1.00  0.00           C  \n"
	"ATOM     79  SG  CYS A   7       4.054  -8.341   6.239  1.00  0.00           S  \n"
	"ATOM     80  H   CYS A   7       1.748  -8.619   4.615  1.00  0.00           H  \n"
	"ATOM     81  HA  CYS A   7       0.525  -8.396   7.409  1.00  0.00           H  \n"
	"ATOM     82  HB2 CYS A   7       2.902  -7.533   8.189  1.00  0.00           H  \n"
	"ATOM     83  HB3 CYS A   7       2.680  -9.251   7.950  1.00  0.00           H  \n"
	"ATOM     84  N   ASN A   8       1.237  -5.569   7.198  1.00  0.00           N  \n"
	"ATOM     85  CA  ASN A   8       0.878  -4.126   6.975  1.00  0.00           C  \n"
	"ATOM     86  C   ASN A   8       2.149  -3.263   6.646  1.00  0.00           C  \n"
	"ATOM     87  O   ASN A   8       3.221  -3.464   7.238  1.00  0.00           O  \n"
	"ATOM     88  CB  ASN A   8       0.101  -3.633   8.234  1.00  0.00           C  \n"
	"ATOM     89  CG  ASN A   8      -0.467  -2.209   8.185  1.00  0.00           C  \n"
	"ATOM     90  OD1 ASN A   8       0.182  -1.254   8.590  1.00  0.00           O  \n"
	"ATOM     91  ND2 ASN A   8      -1.673  -1.986   7.728  1.00  0.00           N  \n"
	"ATOM     92  H   ASN A   8       1.840  -5.876   7.971  1.00  0.00           H  \n"
	"ATOM     93  HA  ASN A   8       0.168  -4.038   6.128  1.00  0.00           H  \n"
	"ATOM     94  HB2 ASN A   8      -0.716  -4.338   8.482  1.00  0.00           H  \n"
	"ATOM     95  HB3 ASN A   8       0.762  -3.653   9.118  1.00  0.00           H  \n"
	"ATOM     96 HD21 ASN A   8      -2.114  -2.731   7.179  1.00  0.00           H  \n"
	"ATOM     97 HD22 ASN A   8      -1.871  -0.987   7.633  1.00  0.00           H  \n"
	"ATOM     98  N   ASP A   9       1.998  -2.296   5.716  1.00  0.00           N  \n"
	"ATOM     99  CA  ASP A   9       3.098  -1.374   5.306  1.00  0.00           C  \n"
	"ATOM    100  C   ASP A   9       3.365  -0.238   6.335  1.00  0.00           C  \n"
	"ATOM    101  O   ASP A   9       4.398  -0.297   7.009  1.00  0.00           O  \n"
	"ATOM    102  CB  ASP A   9       3.021  -0.921   3.812  1.00  0.00           C  \n"
	"ATOM    103  CG  ASP A   9       1.729  -0.269   3.265  1.00  0.00           C  \n"
	"ATOM    104  OD1 ASP A   9       0.764  -0.071   4.009  1.00  0.00           O  \n"
	"ATOM    105  H   ASP A   9       1.058  -2.234   5.308  1.00  0.00           H  \n"
	"ATOM    106  HA  ASP A   9       4.027  -1.972   5.319  1.00  0.00           H  \n"
	"ATOM    107  HB2 ASP A   9       3.885  -0.266   3.605  1.00  0.00           H  \n"
	"ATOM    108  HB3 ASP A   9       3.227  -1.798   3.175  1.00  0.00           H  \n"
	"ATOM    109  N   PHE A  10       2.486   0.779   6.469  1.00  0.00           N  \n"
	"ATOM    110  CA  PHE A  10       2.722   1.912   7.413  1.00  0.00           C  \n"
	"ATOM    111  C   PHE A  10       1.416   2.425   8.114  1.00  0.00           C  \n"
	"ATOM    112  O   PHE A  10       0.837   3.448   7.743  1.00  0.00           O  \n"
	"ATOM    113  CB  PHE A  10       3.557   3.013   6.685  1.00  0.00           C  \n"
	"ATOM    114  CG  PHE A  10       4.297   3.974   7.629  1.00  0.00           C  \n"
	"ATOM    115  CD1 PHE A  10       5.566   3.641   8.115  1.00  0.00           C  \n"
	"ATOM    116  CD2 PHE A  10       3.707   5.180   8.021  1.00  0.00           C  \n"
	"ATOM    117  CE1 PHE A  10       6.234   4.501   8.983  1.00  0.00           C  \n"
	"ATOM    118  CE2 PHE A  10       4.375   6.039   8.892  1.00  0.00           C  \n"
	"ATOM    119  CZ  PHE A  10       5.638   5.699   9.371  1.00  0.00           C  \n"
	"ATOM    120  H   PHE A  10       1.667   0.734   5.853  1.00  0.00           H  \n"
	"ATOM    121  HA  PHE A  10       3.367   1.549   8.241  1.00  0.00           H  \n"
	"ATOM    122  HB2 PHE A  10       4.307   2.543   6.018  1.00  0.00           H  \n"
	"ATOM    123  HB3 PHE A  10       2.914   3.582   5.988  1.00  0.00           H  \n"
	"ATOM    124  HD1 PHE A  10       6.035   2.711   7.822  1.00  0.00           H  \n"
	"ATOM    125  HD2 PHE A  10       2.722   5.448   7.663  1.00  0.00           H  \n"
	"ATOM    126  HE1 PHE A  10       7.214   4.239   9.356  1.00  0.00           H  \n"
	"ATOM    127  HE2 PHE A  10       3.912   6.967   9.195  1.00  0.00           H  \n"
	"ATOM    128  HZ  PHE A  10       6.156   6.366  10.046  1.00  0.00           H  \n"
	"ATOM    129  N   ALA A  11       1.003   1.718   9.183  1.00  0.00           N  \n"
	"ATOM    130  CA  ALA A  11      -0.088   2.128  10.118  1.00  0.00           C  \n"
	"ATOM    131  C   ALA A  11      -1.513   2.327   9.503  1.00  0.00           C  \n"
	"ATOM    132  O   ALA A  11      -1.976   3.450   9.291  1.00  0.00           O  \n"
	"ATOM    133  CB  ALA A  11       0.411   3.311  10.982  1.00  0.00           C  \n"
	"ATOM    134  H   ALA A  11       1.479   0.816   9.249  1.00  0.00           H  \n"
	"ATOM    135  HA  ALA A  11      -0.197   1.292  10.836  1.00  0.00           H  \n"
	"ATOM    136  HB1 ALA A  11       1.373   3.091  11.481  1.00  0.00           H  \n"
	"ATOM    137  HB2 ALA A  11       0.561   4.221  10.370  1.00  0.00           H  \n"
	"ATOM    138  HB3 ALA A  11      -0.316   3.574  11.772  1.00  0.00           H  \n"
	"ATOM    139  N   GLY A  12      -2.216   1.218   9.208  1.00  0.00           N  \n"
	"ATOM    140  CA  GLY A  12      -3.573   1.284   8.578  1.00  0.00           C  \n"
	"ATOM    141  C   GLY A  12      -3.709   1.558   7.056  1.00  0.00           C  \n"
	"ATOM    142  O   GLY A  12      -4.803   1.373   6.525  1.00  0.00           O  \n"
	"ATOM    143  H   GLY A  12      -1.723   0.345   9.426  1.00  0.00           H  \n"
	"ATOM    144  HA2 GLY A  12      -4.100   0.334   8.779  1.00  0.00           H  \n"
	"ATOM    145  HA3 GLY A  12      -4.192   2.045   9.092  1.00  0.00           H  \n"
	"ATOM    146  N   CYS A  13      -2.647   1.987   6.355  1.00  0.00           N  \n"
	"ATOM    147  CA  CYS A  13      -2.716   2.333   4.900  1.00  0.00           C  \n"
	"ATOM    148  C   CYS A  13      -2.953   1.144   3.894  1.00  0.00           C  \n"
	"ATOM    149  O   CYS A  13      -3.682   1.307   2.913  1.00  0.00           O  \n"
	"ATOM    150  CB  CYS A  13      -1.442   3.157   4.605  1.00  0.00           C  \n"
	"ATOM    151  SG  CYS A  13      -1.496   3.979   2.996  1.00  0.00           S  \n"
	"ATOM    152  H   CYS A  13      -1.809   2.116   6.944  1.00  0.00           H  \n"
	"ATOM    153  HA  CYS A  13      -3.577   3.016   4.755  1.00  0.00           H  \n"
	"ATOM    154  HB2 CYS A  13      -1.286   3.963   5.351  1.00  0.00           H  \n"
	"ATOM    155  HB3 CYS A  13      -0.543   2.516   4.645  1.00  0.00           H  \n"
	"ATOM    156  N   GLY A  14      -2.361  -0.039   4.129  1.00  0.00           N  \n"
	"ATOM    157  CA  GLY A  14      -2.615  -1.249   3.303  1.00  0.00           C  \n"
	"ATOM    158  C   GLY A  14      -1.735  -2.452   3.686  1.00  0.00           C  \n"
	"ATOM    159  O   GLY A  14      -1.625  -2.812   4.859  1.00  0.00           O  \n"
	"ATOM    160  H   GLY A  14      -1.808  -0.049   4.991  1.00  0.00           H  \n"
	"ATOM    161  HA2 GLY A  14      -3.668  -1.564   3.433  1.00  0.00           H  \n"
	"ATOM    162  HA3 GLY A  14      -2.524  -1.027   2.221  1.00  0.00           H  \n"
	"ATOM    163  N   TYR A  15      -1.152  -3.105   2.679  1.00  0.00           N  \n"
	"ATOM    164  CA  TYR A  15      -0.423  -4.393   2.855  1.00  0.00           C  \n"
	"ATOM    165  C   TYR A  15       1.122  -4.308   2.592  1.00  0.00           C  \n"
	"ATOM    166  O   TYR A  15       1.634  -3.351   2.009  1.00  0.00           O  \n"
	"ATOM    167  CB  TYR A  15      -1.095  -5.434   1.914  1.00  0.00           C  \n"
	"ATOM    168  CG  TYR A  15      -2.546  -5.851   2.214  1.00  0.00           C  \n"
	"ATOM    169  CD1 TYR A  15      -3.621  -5.100   1.726  1.00  0.00           C  \n"
	"ATOM    170  CD2 TYR A  15      -2.799  -7.052   2.878  1.00  0.00           C  \n"
	"ATOM    171  CE1 TYR A  15      -4.923  -5.575   1.838  1.00  0.00           C  \n"
	"ATOM    172  CE2 TYR A  15      -4.105  -7.530   2.989  1.00  0.00           C  \n"
	"ATOM    173  CZ  TYR A  15      -5.164  -6.793   2.464  1.00  0.00           C  \n"
	"ATOM    174  OH  TYR A  15      -6.443  -7.269   2.549  1.00  0.00           O  \n"
	"ATOM    175  H   TYR A  15      -1.419  -2.768   1.746  1.00  0.00           H  \n"
	"ATOM    176  HA  TYR A  15      -0.557  -4.752   3.888  1.00  0.00           H  \n"
	"ATOM    177  HB2 TYR A  15      -1.027  -5.070   0.876  1.00  0.00           H  \n"
	"ATOM    178  HB3 TYR A  15      -0.479  -6.353   1.889  1.00  0.00           H  \n"
	"ATOM    179  HD1 TYR A  15      -3.458  -4.144   1.256  1.00  0.00           H  \n"
	"ATOM    180  HD2 TYR A  15      -1.976  -7.625   3.278  1.00  0.00           H  \n"
	"ATOM    181  HE1 TYR A  15      -5.739  -5.007   1.410  1.00  0.00           H  \n"
	"ATOM    182  HE2 TYR A  15      -4.285  -8.487   3.448  1.00  0.00           H  \n"
	"ATOM    183  HH  TYR A  15      -6.413  -8.164   2.891  1.00  0.00           H  \n"
	"ATOM    184  N   ALA A  16       1.872  -5.350   2.992  1.00  0.00           N  \n"
	"ATOM    185  CA  ALA A  16       3.334  -5.461   2.750  1.00  0.00           C  \n"
	"ATOM    186  C   ALA A  16       3.775  -6.955   2.655  1.00  0.00           C  \n"
	"ATOM    187  O   ALA A  16       3.299  -7.808   3.411  1.00  0.00           O  \n"
	"ATOM    188  CB  ALA A  16       4.094  -4.761   3.895  1.00  0.00           C  \n"
	"ATOM    189  H   ALA A  16       1.351  -6.113   3.440  1.00  0.00           H  \n"
	"ATOM    190  HA  ALA A  16       3.603  -4.938   1.808  1.00  0.00           H  \n"
	"ATOM    191  HB1 ALA A  16       3.843  -5.170   4.891  1.00  0.00           H  \n"
	"ATOM    192  HB2 ALA A  16       5.192  -4.825   3.776  1.00  0.00           H  \n"
	"ATOM    193  HB3 ALA A  16       3.860  -3.689   3.918  1.00  0.00           H  \n"
	"ATOM    194  N   VAL A  17       4.737  -7.259   1.770  1.00  0.00           N  \n"
	"ATOM    195  CA  VAL A  17       5.423  -8.600   1.753  1.00  0.00           C  \n"
	"ATOM    196  C   VAL A  17       6.603  -8.659   2.810  1.00  0.00           C  \n"
	"ATOM    197  O   VAL A  17       6.749  -9.658   3.517  1.00  0.00           O  \n"
	"ATOM    198  CB  VAL A  17       5.886  -9.007   0.306  1.00  0.00           C  \n"
	"ATOM    199  CG1 VAL A  17       6.481 -10.435   0.238  1.00  0.00           C  \n"
	"ATOM    200  CG2 VAL A  17       4.776  -8.959  -0.771  1.00  0.00           C  \n"
	"ATOM    201  H   VAL A  17       5.059  -6.472   1.201  1.00  0.00           H  \n"
	"ATOM    202  HA  VAL A  17       4.685  -9.372   2.064  1.00  0.00           H  \n"
	"ATOM    203  HB  VAL A  17       6.681  -8.303  -0.009  1.00  0.00           H  \n"
	"ATOM    204 HG11 VAL A  17       7.358 -10.553   0.903  1.00  0.00           H  \n"
	"ATOM    205 HG12 VAL A  17       5.749 -11.209   0.540  1.00  0.00           H  \n"
	"ATOM    206 HG13 VAL A  17       6.833 -10.698  -0.777  1.00  0.00           H  \n"
	"ATOM    207 HG21 VAL A  17       3.910  -9.595  -0.506  1.00  0.00           H  \n"
	"ATOM    208 HG22 VAL A  17       4.400  -7.932  -0.915  1.00  0.00           H  \n"
	"ATOM    209 HG23 VAL A  17       5.136  -9.292  -1.764  1.00  0.00           H  \n"
	"ATOM    210  N   VAL A  18       7.426  -7.595   2.934  1.00  0.00           N  \n"
	"ATOM    211  CA  VAL A  18       8.496  -7.466   3.960  1.00  0.00           C  \n"
	"ATOM    212  C   VAL A  18       7.827  -6.763   5.189  1.00  0.00           C  \n"
	"ATOM    213  O   VAL A  18       7.485  -5.576   5.181  1.00  0.00           O  \n"
	"ATOM    214  CB  VAL A  18       9.750  -6.703   3.411  1.00  0.00           C  \n"
	"ATOM    215  CG1 VAL A  18      10.857  -6.556   4.482  1.00  0.00           C  \n"
	"ATOM    216  CG2 VAL A  18      10.411  -7.375   2.183  1.00  0.00           C  \n"
	"ATOM    217  H   VAL A  18       7.092  -6.762   2.463  1.00  0.00           H  \n"
	"ATOM    218  HA  VAL A  18       8.857  -8.478   4.245  1.00  0.00           H  \n"
	"ATOM    219  HB  VAL A  18       9.431  -5.682   3.114  1.00  0.00           H  \n"
	"ATOM    220 HG11 VAL A  18      10.499  -5.998   5.370  1.00  0.00           H  \n"
	"ATOM    221 HG12 VAL A  18      11.218  -7.536   4.848  1.00  0.00           H  \n"
	"ATOM    222 HG13 VAL A  18      11.735  -6.000   4.104  1.00  0.00           H  \n"
	"ATOM    223 HG21 VAL A  18       9.709  -7.463   1.334  1.00  0.00           H  \n"
	"ATOM    224 HG22 VAL A  18      11.280  -6.799   1.811  1.00  0.00           H  \n"
	"ATOM    225 HG23 VAL A  18      10.769  -8.398   2.410  1.00  0.00           H  \n"
	"ATOM    226  N   CYS A  19       7.596  -7.575   6.212  1.00  0.00           N  \n"
	"ATOM    227  CA  CYS A  19       6.831  -7.189   7.421  1.00  0.00           C  \n"
	"ATOM    228  C   CYS A  19       7.504  -6.163   8.392  1.00  0.00           C  \n"
	"ATOM    229  O   CYS A  19       8.476  -6.462   9.090  1.00  0.00           O  \n"
	"ATOM    230  CB  CYS A  19       6.458  -8.514   8.115  1.00  0.00           C  \n"
	"ATOM    231  SG  CYS A  19       5.468  -9.524   7.004  1.00  0.00           S  \n"
	"ATOM    232  H   CYS A  19       7.819  -8.533   5.935  1.00  0.00           H  \n"
	"ATOM    233  HA  CYS A  19       5.869  -6.752   7.084  1.00  0.00           H  \n"
	"ATOM    234  HB2 CYS A  19       7.351  -9.073   8.456  1.00  0.00           H  \n"
	"ATOM    235  HB3 CYS A  19       5.845  -8.320   9.017  1.00  0.00           H  \n"
	"ATOM    236  N   PHE A  20       6.915  -4.960   8.466  1.00  0.00           N  \n"
	"ATOM    237  CA  PHE A  20       7.340  -3.890   9.428  1.00  0.00           C  \n"
	"ATOM    238  C   PHE A  20       7.043  -4.163  10.951  1.00  0.00           C  \n"
	"ATOM    239  O   PHE A  20       7.764  -3.684  11.828  1.00  0.00           O  \n"
	"ATOM    240  CB  PHE A  20       6.737  -2.528   8.965  1.00  0.00           C  \n"
	"ATOM    241  CG  PHE A  20       7.252  -1.968   7.622  1.00  0.00           C  \n"
	"ATOM    242  CD1 PHE A  20       6.676  -2.379   6.415  1.00  0.00           C  \n"
	"ATOM    243  CD2 PHE A  20       8.285  -1.025   7.599  1.00  0.00           C  \n"
	"ATOM    244  CE1 PHE A  20       7.122  -1.855   5.205  1.00  0.00           C  \n"
	"ATOM    245  CE2 PHE A  20       8.731  -0.499   6.387  1.00  0.00           C  \n"
	"ATOM    246  CZ  PHE A  20       8.148  -0.914   5.192  1.00  0.00           C  \n"
	"ATOM    247  H   PHE A  20       6.172  -4.820   7.775  1.00  0.00           H  \n"
	"ATOM    248  HA  PHE A  20       8.438  -3.810   9.382  1.00  0.00           H  \n"
	"ATOM    249  HB2 PHE A  20       5.630  -2.582   8.944  1.00  0.00           H  \n"
	"ATOM    250  HB3 PHE A  20       6.931  -1.772   9.751  1.00  0.00           H  \n"
	"ATOM    251  HD1 PHE A  20       5.872  -3.102   6.411  1.00  0.00           H  \n"
	"ATOM    252  HD2 PHE A  20       8.745  -0.695   8.520  1.00  0.00           H  \n"
	"ATOM    253  HE1 PHE A  20       6.668  -2.178   4.279  1.00  0.00           H  \n"
	"ATOM    254  HE2 PHE A  20       9.529   0.230   6.375  1.00  0.00           H  \n"
	"ATOM    255  HZ  PHE A  20       8.493  -0.506   4.253  1.00  0.00           H  \n"
	"ATOM    256  N   TRP A  21       5.998  -4.946  11.245  1.00  0.00           N  \n"
	"ATOM    257  CA  TRP A  21       5.671  -5.449  12.606  1.00  0.00           C  \n"
	"ATOM    258  C   TRP A  21       6.148  -6.923  12.789  1.00  0.00           C  \n"
	"ATOM    259  O   TRP A  21       6.075  -7.802  11.931  1.00  0.00           O  \n"
	"ATOM    260  CB  TRP A  21       4.170  -5.169  12.901  1.00  0.00           C  \n"
	"ATOM    261  CG  TRP A  21       3.106  -5.880  12.041  1.00  0.00           C  \n"
	"ATOM    262  CD1 TRP A  21       2.347  -5.265  11.023  1.00  0.00           C  \n"
	"ATOM    263  CD2 TRP A  21       2.650  -7.187  12.100  1.00  0.00           C  \n"
	"ATOM    264  NE1 TRP A  21       1.419  -6.157  10.446  1.00  0.00           N  \n"
	"ATOM    265  CE2 TRP A  21       1.631  -7.342  11.126  1.00  0.00           C  \n"
	"ATOM    266  CE3 TRP A  21       3.031  -8.282  12.917  1.00  0.00           C  \n"
	"ATOM    267  CZ2 TRP A  21       0.989  -8.590  10.959  1.00  0.00           C  \n"
	"ATOM    268  CZ3 TRP A  21       2.387  -9.507  12.732  1.00  0.00           C  \n"
	"ATOM    269  CH2 TRP A  21       1.381  -9.659  11.769  1.00  0.00           C  \n"
	"ATOM    270  OXT TRP A  21       6.655  -7.159  14.032  1.00  0.00           O  \n"
	"ATOM    271  H   TRP A  21       5.586  -5.321  10.387  1.00  0.00           H  \n"
	"ATOM    272  HA  TRP A  21       6.223  -4.882  13.373  1.00  0.00           H  \n"
	"ATOM    273  HB2 TRP A  21       3.966  -5.364  13.970  1.00  0.00           H  \n"
	"ATOM    274  HB3 TRP A  21       4.009  -4.077  12.825  1.00  0.00           H  \n"
	"ATOM    275  HD1 TRP A  21       2.457  -4.227  10.739  1.00  0.00           H  \n"
	"ATOM    276  HE1 TRP A  21       0.648  -5.953   9.800  1.00  0.00           H  \n"
	"ATOM    277  HE3 TRP A  21       3.831  -8.176  13.639  1.00  0.00           H  \n"
	"ATOM    278  HZ2 TRP A  21       0.216  -8.717  10.217  1.00  0.00           H  \n"
	"ATOM    279  HZ3 TRP A  21       2.677 -10.356  13.336  1.00  0.00           H  \n"
	"ATOM    280  HH2 TRP A  21       0.904 -10.621  11.646  1.00  0.00           H  \n"
	"TER     281      TRP A  21                                                      \n";
}

#endif
