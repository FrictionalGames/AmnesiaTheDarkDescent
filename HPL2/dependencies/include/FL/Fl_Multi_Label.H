//
// "$Id: Fl_Multi_Label.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Multi-label header file for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Multi_Label_H
#define Fl_Multi_Label_H

class Fl_Widget;
struct Fl_Menu_Item;

struct FL_EXPORT Fl_Multi_Label {
  const char* labela;
  const char* labelb;
  uchar typea;
  uchar typeb;
  void label(Fl_Widget*);
  void label(Fl_Menu_Item*);
};

#endif

//
// End of "$Id: Fl_Multi_Label.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
