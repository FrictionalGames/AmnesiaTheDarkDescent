//
// "$Id: Fl_JPEG_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// JPEG image header file for the Fast Light Tool Kit (FLTK).
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
   Fl_JPEG_Image class . */

#ifndef Fl_JPEG_Image_H
#define Fl_JPEG_Image_H
#  include "Fl_Image.H"

/**
  The Fl_JPEG_Image class supports loading, caching,
  and drawing of Joint Photographic Experts Group (JPEG) File
  Interchange Format (JFIF) images. The class supports grayscale
  and color (RGB) JPEG image files.
*/
class FL_EXPORT Fl_JPEG_Image : public Fl_RGB_Image {

public:

  Fl_JPEG_Image(const char *filename);
  Fl_JPEG_Image(const char *name, const unsigned char *data);
};

#endif

//
// End of "$Id: Fl_JPEG_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
