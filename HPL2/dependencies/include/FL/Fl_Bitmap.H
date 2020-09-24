//
// "$Id: Fl_Bitmap.H 10110 2014-02-22 14:51:50Z manolo $"
//
// Bitmap header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Bitmap widget . */

#ifndef Fl_Bitmap_H
#define Fl_Bitmap_H
#  include "Fl_Image.H"

class Fl_Widget;
struct Fl_Menu_Item;

/**
  The Fl_Bitmap class supports caching and drawing of mono-color
  (bitmap) images. Images are drawn using the current color.
*/
class FL_EXPORT Fl_Bitmap : public Fl_Image {
  friend class Fl_Quartz_Graphics_Driver;
  friend class Fl_GDI_Graphics_Driver;
  friend class Fl_GDI_Printer_Graphics_Driver;
  friend class Fl_Xlib_Graphics_Driver;
public:

  /** pointer to raw bitmap data */
  const uchar *array;
  /** Non-zero if array points to bitmap data allocated internally */
  int alloc_array;
  
  private:
  int start(int XP, int YP, int WP, int HP, int &cx, int &cy, 
	    int &X, int &Y, int &W, int &H);
#if defined(__APPLE__) || defined(WIN32)
  /** for internal use */
  void *id_;
#else
  /** for internal use */
  unsigned id_;
#endif // __APPLE__ || WIN32

  public:

  /** The constructors create a new bitmap from the specified bitmap data */
  Fl_Bitmap(const uchar *bits, int W, int H) :
    Fl_Image(W,H,0), array(bits), alloc_array(0), id_(0) {data((const char **)&array, 1);}
  /** The constructors create a new bitmap from the specified bitmap data */
  Fl_Bitmap(const char *bits, int W, int H) :
    Fl_Image(W,H,0), array((const uchar *)bits), alloc_array(0), id_(0) {data((const char **)&array, 1);}
  virtual ~Fl_Bitmap();
  virtual Fl_Image *copy(int W, int H);
  Fl_Image *copy() { return copy(w(), h()); }
  virtual void draw(int X, int Y, int W, int H, int cx=0, int cy=0);
  void draw(int X, int Y) {draw(X, Y, w(), h(), 0, 0);}
  virtual void label(Fl_Widget*w);
  virtual void label(Fl_Menu_Item*m);
  virtual void uncache();
};

#endif

//
// End of "$Id: Fl_Bitmap.H 10110 2014-02-22 14:51:50Z manolo $".
//
