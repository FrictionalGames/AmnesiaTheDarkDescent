//
// "$Id: Fl_FormsBitmap.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Forms bitmap header file for the Fast Light Tool Kit (FLTK).
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
   Fl_FormsBitmap widget . */

#ifndef Fl_FormsBitmap_H
#define Fl_FormsBitmap_H

#include "Fl_Bitmap.H"

/** 
    Forms compatibility Bitmap Image Widget
*/
class FL_EXPORT Fl_FormsBitmap : public Fl_Widget {
    Fl_Bitmap *b;
protected:
    void draw();
public:
    Fl_FormsBitmap(Fl_Boxtype, int, int, int, int, const char * = 0);
    void set(int W, int H, const uchar *bits);
    /** Sets a new bitmap. */
    void bitmap(Fl_Bitmap *B) {b = B;}
    /** Gets a the current associated Fl_Bitmap objects. */
    Fl_Bitmap *bitmap() const {return b;}
};

#endif

//
// End of "$Id: Fl_FormsBitmap.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
