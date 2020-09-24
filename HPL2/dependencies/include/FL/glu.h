//
// "$Id: glu.h 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// GLu header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// You must include this instead of GL/gl.h to get the Microsoft
// APIENTRY stuff included (from <windows.h>) prior to the OpenGL
// header files.
//
// This file also provides "missing" OpenGL functions, and
// gl_start() and gl_finish() to allow OpenGL to be used in any window
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

#ifndef FL_glu_H
#  define FL_glu_H

#  include "Enumerations.H" // for color names
#  ifdef WIN32
#    include <windows.h>
#  endif
#  ifndef APIENTRY
#    if defined(__CYGWIN__)
#      define APIENTRY __attribute__ ((__stdcall__))
#    else
#      define APIENTRY
#    endif
#  endif

#  ifdef __APPLE__
#    include <OpenGL/glu.h>
#  else
#    include <GL/glu.h>
#  endif

#endif // !FL_glu_H

//
// End of "$Id: glu.h 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
