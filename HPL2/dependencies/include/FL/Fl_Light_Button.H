//
// "$Id: Fl_Light_Button.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Lighted button header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Light_Button widget . */

#ifndef Fl_Light_Button_H
#define Fl_Light_Button_H

#include "Fl_Button.H"

/**
  This subclass displays the "on" state by turning on a light, 
  rather than drawing pushed in.  The shape of the  "light" 
  is initially set to FL_DOWN_BOX.  The color of the light when
  on is controlled with selection_color(), which defaults to FL_YELLOW.

  Buttons generate callbacks when they are clicked by the user.  You
  control exactly when and how by changing the values for type() and when().
  <P ALIGN=CENTER>\image html Fl_Light_Button.png</P> 
  \image latex Fl_Light_Button.png "Fl_Light_Button" width=4cm
*/
class FL_EXPORT Fl_Light_Button : public Fl_Button {
protected:
    virtual void draw();
public:
    virtual int handle(int);
    Fl_Light_Button(int x,int y,int w,int h,const char *l = 0);
};

#endif

//
// End of "$Id: Fl_Light_Button.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
