//
// "$Id: Fl_FormsPixmap.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Forms pixmap header file for the Fast Light Tool Kit (FLTK).
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
   Fl_FormsPixmap widget . */

#ifndef Fl_FormsPixmap_H
#define Fl_FormsPixmap_H

#include "Fl_Pixmap.H"

/**
  \class Fl_FormsPixmap
  \brief Forms pixmap drawing routines
*/
class FL_EXPORT Fl_FormsPixmap : public Fl_Widget {
    Fl_Pixmap *b;
protected:
    void draw();
public:
    Fl_FormsPixmap(Fl_Boxtype t, int X, int Y, int W, int H, const char *L= 0);

    void set(/*const*/char * const * bits);

    /**
      Set the internal pixmap pointer to an existing pixmap.
      \param[in] B existing pixmap
    */
    void Pixmap(Fl_Pixmap *B) {b = B;}

    /** Get the internal pixmap pointer. */
    Fl_Pixmap *Pixmap() const {return b;}
};

#endif

//
// End of "$Id: Fl_FormsPixmap.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
