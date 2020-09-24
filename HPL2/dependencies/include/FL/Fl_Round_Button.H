//
// "$Id: Fl_Round_Button.H 10386 2014-10-19 20:17:17Z AlbrechtS $"
//
// Round button header file for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Round_Button_H
#define Fl_Round_Button_H

#include "Fl_Light_Button.H"

/**
  Buttons generate callbacks when they are clicked by the user.  You
  control exactly when and how by changing the values for type()
  and when().
  <P ALIGN=CENTER>\image html Fl_Round_Button.png</P>
  \image latex  Fl_Round_Button.png " Fl_Round_Button" width=4cm
  <P>The Fl_Round_Button subclass display the "on" state by
  turning on a light, rather than drawing pushed in.  The shape of the
  "light" is initially set to FL_ROUND_DOWN_BOX.  The color of the light
  when on is controlled with selection_color(), which defaults to
  FL_FOREGROUND_COLOR.
*/
class FL_EXPORT Fl_Round_Button : public Fl_Light_Button {
public:
  Fl_Round_Button(int x,int y,int w,int h,const char *l = 0);
};

#endif

//
// End of "$Id: Fl_Round_Button.H 10386 2014-10-19 20:17:17Z AlbrechtS $".
//
