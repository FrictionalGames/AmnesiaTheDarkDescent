//
// "$Id: Fl_Single_Window.H 9637 2012-07-24 04:37:22Z matt $"
//
// Single-buffered window header file for the Fast Light Tool Kit (FLTK).
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

/* \file
   Fl_Single_Window class . */

#ifndef Fl_Single_Window_H
#define Fl_Single_Window_H

#include "Fl_Window.H"

/**
  This is the same as Fl_Window.  However, it is possible that
  some implementations will provide double-buffered windows by default.
  This subcan be used to force single-buffering.  This may be
  useful for modifying existing programs that use incremental update, or
  for some types of image data, such as a movie flipbook.
*/
class FL_EXPORT Fl_Single_Window : public Fl_Window {
public:
  void show();
  void show(int a, char **b) {Fl_Window::show(a,b);}
  void flush();
   /**
    Creates a new Fl_Single_Window widget using the given
    size, and label (title) string.
  */
  Fl_Single_Window(int W, int H, const char *l=0);

  /**
    Creates a new Fl_Single_Window widget using the given
    position, size, and label (title) string.
  */
  Fl_Single_Window(int X, int Y, int W, int H, const char *l=0);
  
  int make_current();
};

#endif

//
// End of "$Id: Fl_Single_Window.H 9637 2012-07-24 04:37:22Z matt $".
//
