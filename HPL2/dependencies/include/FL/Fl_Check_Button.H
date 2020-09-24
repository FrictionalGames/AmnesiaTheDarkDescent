//
// "$Id: Fl_Check_Button.H 10386 2014-10-19 20:17:17Z AlbrechtS $"
//
// Check button header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2014 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file. If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

#ifndef Fl_Check_Button_H
#define Fl_Check_Button_H

#include "Fl_Light_Button.H"

/*
  class: Fl_Check_Button.

  A button with a "checkmark" to show its status.
*/

class FL_EXPORT Fl_Check_Button : public Fl_Light_Button {
public:
  Fl_Check_Button(int X, int Y, int W, int H, const char *L = 0);
};

#endif

//
// End of "$Id: Fl_Check_Button.H 10386 2014-10-19 20:17:17Z AlbrechtS $".
//
