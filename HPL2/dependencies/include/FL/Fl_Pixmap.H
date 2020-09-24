//
// "$Id: Fl_Pixmap.H 9706 2012-11-06 20:46:14Z matt $"
//
// Pixmap header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2012 by Bill Spitzak and others.
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
   Fl_Pixmap widget . */

#ifndef Fl_Pixmap_H
#define Fl_Pixmap_H
#  include "Fl_Image.H"
#if defined(WIN32)
#  include "x.H"
#endif

class Fl_Widget;
struct Fl_Menu_Item;

// Older C++ compilers don't support the explicit keyword... :(
#  if defined(__sgi) && !defined(_COMPILER_VERSION)
#    define explicit
#  endif // __sgi && !_COMPILER_VERSION

/**
  The Fl_Pixmap class supports caching and drawing of colormap
  (pixmap) images, including transparency.
*/
class FL_EXPORT Fl_Pixmap : public Fl_Image {
  friend class Fl_Quartz_Graphics_Driver;
  friend class Fl_GDI_Graphics_Driver;
  friend class Fl_GDI_Printer_Graphics_Driver;
  friend class Fl_Xlib_Graphics_Driver;
  void copy_data();
  void delete_data();
  void set_data(const char * const *p);
  int prepare(int XP, int YP, int WP, int HP, int &cx, int &cy,
			 int &X, int &Y, int &W, int &H);

  protected:

  void measure();

  public:

  int alloc_data; // Non-zero if data was allocated
  
  private:

#if defined(WIN32)
#if FLTK_ABI_VERSION < 10301
  static // a static member is needed for ABI compatibility
#endif
  UINT pixmap_bg_color; // RGB color used for pixmap background
#endif // WIN32
#if defined(__APPLE__) || defined(WIN32)
  void *id_; // for internal use
  void *mask_; // for internal use (mask bitmap)
#else
  unsigned id_; // for internal use
  unsigned mask_; // for internal use (mask bitmap)
#endif // __APPLE__ || WIN32
  
  public:

  /**    The constructors create a new pixmap from the specified XPM data.  */
  explicit Fl_Pixmap(char * const * D) : Fl_Image(-1,0,1), alloc_data(0), id_(0), mask_(0) {set_data((const char*const*)D); measure();}
  /**    The constructors create a new pixmap from the specified XPM data.  */
  explicit Fl_Pixmap(uchar* const * D) : Fl_Image(-1,0,1), alloc_data(0), id_(0), mask_(0) {set_data((const char*const*)D); measure();}
  /**    The constructors create a new pixmap from the specified XPM data.  */
  explicit Fl_Pixmap(const char * const * D) : Fl_Image(-1,0,1), alloc_data(0), id_(0), mask_(0) {set_data((const char*const*)D); measure();}
  /**    The constructors create a new pixmap from the specified XPM data.  */
  explicit Fl_Pixmap(const uchar* const * D) : Fl_Image(-1,0,1), alloc_data(0), id_(0), mask_(0) {set_data((const char*const*)D); measure();}
  virtual ~Fl_Pixmap();
  virtual Fl_Image *copy(int W, int H);
  Fl_Image *copy() { return copy(w(), h()); }
  virtual void color_average(Fl_Color c, float i);
  virtual void desaturate();
  virtual void draw(int X, int Y, int W, int H, int cx=0, int cy=0);
  void draw(int X, int Y) {draw(X, Y, w(), h(), 0, 0);}
  virtual void label(Fl_Widget*w);
  virtual void label(Fl_Menu_Item*m);
  virtual void uncache();
};

#endif

//
// End of "$Id: Fl_Pixmap.H 9706 2012-11-06 20:46:14Z matt $".
//
