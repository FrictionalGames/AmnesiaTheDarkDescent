//
// "$Id: Fl_PNG_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// PNG image header file for the Fast Light Tool Kit (FLTK).
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
   Fl_PNG_Image class . */

#ifndef Fl_PNG_Image_H
#define Fl_PNG_Image_H
#  include "Fl_Image.H"

/**
  The Fl_PNG_Image class supports loading, caching,
  and drawing of Portable Network Graphics (PNG) image files. The
  class loads colormapped and full-color images and handles color-
  and alpha-based transparency.
*/
class FL_EXPORT Fl_PNG_Image : public Fl_RGB_Image {

public:

  Fl_PNG_Image(const char* filename);
  Fl_PNG_Image (const char *name_png, const unsigned char *buffer, int datasize);
private:
  void load_png_(const char *name_png, const unsigned char *buffer_png, int datasize);
};

#endif

//
// End of "$Id: Fl_PNG_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
