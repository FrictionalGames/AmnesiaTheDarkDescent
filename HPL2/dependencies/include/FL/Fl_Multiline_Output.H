//
// "$Id: Fl_Multiline_Output.H 9637 2012-07-24 04:37:22Z matt $"
//
// Multi line output header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2011 by Bill Spitzak and others.
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
   Fl_Multiline_Output widget . */

#ifndef Fl_Multiline_Output_H
#define Fl_Multiline_Output_H

#include "Fl_Output.H"

/**
  This widget is a subclass of Fl_Output that displays multiple
  lines of text. It also displays tab characters as whitespace to the
  next column.

  Note that this widget does not support scrollbars, or per-character
  color control.

  If you are presenting large amounts of read-only text 
  and need scrollbars, or full color control of characters,
  then use Fl_Text_Display. If you want to display HTML text,
  use Fl_Help_View.
*/
class FL_EXPORT Fl_Multiline_Output : public Fl_Output {
public:

  /**
    Creates a new Fl_Multiline_Output widget using the given position,
    size, and label string. The default boxtype is FL_DOWN_BOX.

    Inherited destructor destroys the widget and any value associated with it.
  */
  Fl_Multiline_Output(int X,int Y,int W,int H,const char *l = 0);
};

#endif

//
// End of "$Id: Fl_Multiline_Output.H 9637 2012-07-24 04:37:22Z matt $".
//
