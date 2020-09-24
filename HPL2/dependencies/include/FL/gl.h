//
// "$Id: gl.h 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// OpenGL header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2011 by Bill Spitzak and others.
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

/** \file gl.h
 *  This file defines wrapper functions for OpenGL in FLTK
 *
 *  To use OpenGL from within an FLTK application you MUST use gl_visual()
 *  to select the default visual before doing show() on any windows. Mesa
 *  will crash if yoy try to use a visual not returned by glxChooseVidual.
 *
 *  This does not work with Fl_Double_Window's!  It will try to draw
 *  into the front buffer.  Depending on the system this will either
 *  crash or do nothing (when pixmaps are being used as back buffer
 *  and GL is being done by hardware), work correctly (when GL is done
 *  with software, such as Mesa), or draw into the front buffer and
 *  be erased when the buffers are swapped (when double buffer hardware
 *  is being used)
 */

#ifndef FL_gl_H
#  define FL_gl_H

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
#    include <OpenGL/gl.h>
#  else
#    include <GL/gl.h>
#  endif

FL_EXPORT void gl_start();
FL_EXPORT void gl_finish();

FL_EXPORT void gl_color(Fl_Color i);
/** back compatibility */
inline void gl_color(int c) {gl_color((Fl_Color)c);}

FL_EXPORT void gl_rect(int x,int y,int w,int h);
/**
  Fills the given rectangle with the current color.
  \see gl_rect(int x, int y, int w, int h)
  */
inline void gl_rectf(int x,int y,int w,int h) {glRecti(x,y,x+w,y+h);}

FL_EXPORT void gl_font(int fontid, int size);
FL_EXPORT int  gl_height();
FL_EXPORT int  gl_descent();
FL_EXPORT double gl_width(const char *);
FL_EXPORT double gl_width(const char *, int n);
FL_EXPORT double gl_width(uchar);

FL_EXPORT void gl_draw(const char*);
FL_EXPORT void gl_draw(const char*, int n);
FL_EXPORT void gl_draw(const char*, int x, int y);
FL_EXPORT void gl_draw(const char*, float x, float y);
FL_EXPORT void gl_draw(const char*, int n, int x, int y);
FL_EXPORT void gl_draw(const char*, int n, float x, float y);
FL_EXPORT void gl_draw(const char*, int x, int y, int w, int h, Fl_Align);
FL_EXPORT void gl_measure(const char*, int& x, int& y);
#ifdef __APPLE__
extern FL_EXPORT void gl_texture_pile_height(int max);
extern FL_EXPORT int gl_texture_pile_height();
#endif

FL_EXPORT void gl_draw_image(const uchar *, int x,int y,int w,int h, int d=3, int ld=0);

#endif // !FL_gl_H

//
// End of "$Id: gl.h 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
