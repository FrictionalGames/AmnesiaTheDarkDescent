//
// "$Id: Fl_Tiled_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Tiled image header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Tiled_Image widget . */

#ifndef Fl_Tiled_Image_H
#  define Fl_Tiled_Image_H

#  include "Fl_Image.H"


/**
  This class supports tiling of images
  over a specified area. The source (tile) image is <B>not</B>
  copied unless you call the color_average(),
  desaturate(),
  or inactive()
  methods.
*/
class FL_EXPORT Fl_Tiled_Image : public Fl_Image {
  protected:

  Fl_Image	*image_;		// The image that is shared
  int		alloc_image_;		// Did we allocate this image?

  public:

  Fl_Tiled_Image(Fl_Image *i, int W = 0, int H = 0);
  virtual ~Fl_Tiled_Image();

  virtual Fl_Image *copy(int W, int H);
  Fl_Image *copy() { return copy(w(), h()); }
  virtual void color_average(Fl_Color c, float i);
  virtual void desaturate();
  virtual void draw(int X, int Y, int W, int H, int cx, int cy);
  void draw(int X, int Y) { draw(X, Y, w(), h(), 0, 0); }
  /** Gets The image that is shared */ 
  Fl_Image *image() { return image_; }
};

#endif // !Fl_Tiled_Image_H

//
// End of "$Id: Fl_Tiled_Image.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
