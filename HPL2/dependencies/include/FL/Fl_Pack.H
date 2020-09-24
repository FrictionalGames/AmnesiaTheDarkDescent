//
// "$Id: Fl_Pack.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Pack header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Pack widget . */

#ifndef Fl_Pack_H
#define Fl_Pack_H

#include <FL/Fl_Group.H>

/**
  This widget was designed to add the functionality of compressing and
  aligning widgets.
  <P>If type() is Fl_Pack::HORIZONTAL all the children are
  resized to the height of the Fl_Pack, and are moved next to
  each other horizontally. If type() is not Fl_Pack::HORIZONTAL
  then the children are resized to the width and are stacked below each
  other.  Then the Fl_Pack resizes itself to surround the child
  widgets.
  <P>This widget is needed for the Fl_Tabs.
  In addition you may want to put the Fl_Pack inside an 
  Fl_Scroll.

  <P>The resizable for Fl_Pack is set to NULL by default.</p>
  <P>See also: Fl_Group::resizable()
*/
class FL_EXPORT Fl_Pack : public Fl_Group {
  int spacing_;

public:
  enum { // values for type(int)
    VERTICAL = 0,
    HORIZONTAL = 1
  };

protected:
  void draw();

public:
  Fl_Pack(int x,int y,int w ,int h,const char *l = 0);
  /**
    Gets the number of extra pixels of blank space that are added
    between the children.
  */
  int spacing() const {return spacing_;}
  /**
    Sets the number of extra pixels of blank space that are added
    between the children.
  */
  void spacing(int i) {spacing_ = i;}
  /** Same as Fl_Group::type() */
  uchar horizontal() const {return type();}
};

#endif

//
// End of "$Id: Fl_Pack.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
