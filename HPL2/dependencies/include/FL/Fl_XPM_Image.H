//
// "$Id: Fl_XPM_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// XPM image header file for the Fast Light Tool Kit (FLTK).
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
  
   Fl_XPM_Image class . */

#ifndef Fl_XPM_Image_H
#define Fl_XPM_Image_H
#  include "Fl_Pixmap.H"

/**
  The Fl_XPM_Image class supports loading, caching,
  and drawing of X Pixmap (XPM) images, including transparency.
*/
class FL_EXPORT Fl_XPM_Image : public Fl_Pixmap {

  public:

  Fl_XPM_Image(const char* filename);
};

#endif // !Fl_XPM_Image

//
// End of "$Id: Fl_XPM_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
