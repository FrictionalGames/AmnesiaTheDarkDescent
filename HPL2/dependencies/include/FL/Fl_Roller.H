//
// "$Id: Fl_Roller.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Roller header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Roller widget . */

#ifndef Fl_Roller_H
#define Fl_Roller_H

#ifndef Fl_Valuator_H
#include "Fl_Valuator.H"
#endif

/**
  The Fl_Roller widget is a "dolly" control commonly used to
  move 3D objects.
  <P ALIGN=CENTER>\image html Fl_Roller.png 
  \image latex Fl_Roller.png "Fl_Roller" width=4cm
*/
class FL_EXPORT Fl_Roller : public Fl_Valuator {
protected:
  void draw();
public:
  int handle(int);
  Fl_Roller(int X,int Y,int W,int H,const char* L=0);
};

#endif

//
// End of "$Id: Fl_Roller.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
