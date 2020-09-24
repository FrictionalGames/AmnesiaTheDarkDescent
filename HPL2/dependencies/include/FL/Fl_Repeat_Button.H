//
// "$Id: Fl_Repeat_Button.H 9637 2012-07-24 04:37:22Z matt $"
//
// Repeat button header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Repeat_Button widget . */

#ifndef Fl_Repeat_Button_H
#define Fl_Repeat_Button_H
#include "Fl.H"
#include "Fl_Button.H"

/**
  The Fl_Repeat_Button is a subclass of Fl_Button that
  generates a callback when it is pressed and then repeatedly generates
  callbacks as long as it is held down.  The speed of the repeat is fixed
  and depends on the implementation.
*/
class FL_EXPORT Fl_Repeat_Button : public Fl_Button {
  static void repeat_callback(void *);
public:
  int handle(int);
  /**
    Creates a new Fl_Repeat_Button widget using the given
    position, size, and label string. The default boxtype is FL_UP_BOX.
    Deletes the button.
  */
  Fl_Repeat_Button(int X,int Y,int W,int H,const char *l=0);

  void deactivate() {
    Fl::remove_timeout(repeat_callback,this);
    Fl_Button::deactivate();
  }
};

#endif

//
// End of "$Id: Fl_Repeat_Button.H 9637 2012-07-24 04:37:22Z matt $".
//
