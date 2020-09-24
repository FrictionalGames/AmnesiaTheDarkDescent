//
// "$Id: math.h 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Math header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

// Xcode on OS X includes files by recursing down into directories.
// This code catches the cycle and directly includes the required file.
#ifdef fl_math_h_cyclic_include 
#  include "/usr/include/math.h"
#endif

#ifndef fl_math_h
#  define fl_math_h

#  define fl_math_h_cyclic_include 
#  include <math.h>
#  undef fl_math_h_cyclic_include 

#  ifdef __EMX__
#    include <float.h>
#  endif


#  ifndef M_PI
#    define M_PI            3.14159265358979323846
#    define M_PI_2          1.57079632679489661923
#    define M_PI_4          0.78539816339744830962
#    define M_1_PI          0.31830988618379067154
#    define M_2_PI          0.63661977236758134308
#  endif // !M_PI

#  ifndef M_SQRT2
#    define M_SQRT2         1.41421356237309504880
#    define M_SQRT1_2       0.70710678118654752440
#  endif // !M_SQRT2

#  if (defined(WIN32) || defined(CRAY)) && !defined(__MINGW32__) && !defined(__MWERKS__)

inline double rint(double v) {return floor(v+.5);}
inline double copysign(double a, double b) {return b<0 ? -a : a;}

#  endif // (WIN32 || CRAY) && !__MINGW32__ && !__MWERKS__

#endif // !fl_math_h


//
// End of "$Id: math.h 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
