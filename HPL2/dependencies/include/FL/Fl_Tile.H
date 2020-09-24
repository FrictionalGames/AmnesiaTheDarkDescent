//
// "$Id: Fl_Tile.H 10385 2014-10-19 14:17:47Z AlbrechtS $"
//
// Tile header file for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Tile_H
#define Fl_Tile_H

#include "Fl_Group.H"

/*
  The Fl_Tile class lets you resize its children by dragging
  the border between them.
*/

class FL_EXPORT Fl_Tile : public Fl_Group {
public:
  int handle(int);
  Fl_Tile(int X,int Y,int W,int H,const char*l=0);
  void resize(int, int, int, int);
  void position(int, int, int, int);
};

#endif

//
// End of "$Id: Fl_Tile.H 10385 2014-10-19 14:17:47Z AlbrechtS $".
//
