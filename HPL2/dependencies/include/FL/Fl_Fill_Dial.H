//
// "$Id: Fl_Fill_Dial.H 9637 2012-07-24 04:37:22Z matt $"
//
// Filled dial header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Fill_Dial widget . */

#ifndef Fl_Fill_Dial_H
#define Fl_Fill_Dial_H

#include "Fl_Dial.H"

/** Draws a dial with a filled arc */
class FL_EXPORT Fl_Fill_Dial : public Fl_Dial {
public:
  /** Creates a filled dial, also setting its type to FL_FILL_DIAL. */
  Fl_Fill_Dial(int X,int Y,int W,int H, const char *L);
};

#endif

//
// End of "$Id: Fl_Fill_Dial.H 9637 2012-07-24 04:37:22Z matt $".
//
