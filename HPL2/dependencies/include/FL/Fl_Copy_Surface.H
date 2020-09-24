//
// "$Id: Fl_Copy_Surface.H 10406 2014-10-29 19:57:42Z cand $"
//
// Copy-to-clipboard code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2014 by Bill Spitzak and others.
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

#ifndef Fl_Copy_Surface_H
#define Fl_Copy_Surface_H

#include <FL/Fl_Paged_Device.H>
#include <FL/Fl_Printer.H>

#if defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#endif

/** Supports copying of graphical data to the clipboard.
 
 <br> After creation of an Fl_Copy_Surface object, call set_current() on it, and all subsequent graphics requests
 will be recorded in the clipboard. It's possible to draw widgets (using Fl_Copy_Surface::draw()
 ) or to use any of the \ref fl_drawings or the \ref fl_attributes.
 Finally, delete the Fl_Copy_Surface object to load the clipboard with the graphical data.
 <br> Fl_GL_Window 's can be copied to the clipboard as well. 
 <br> Usage example:
 \code
 Fl_Widget *g = ...; // a widget you want to copy to the clipboard
 Fl_Copy_Surface *copy_surf = new Fl_Copy_Surface(g->w(), g->h()); // create an Fl_Copy_Surface object
 copy_surf->set_current(); // direct graphics requests to the clipboard
 fl_color(FL_WHITE); fl_rectf(0, 0, g->w(), g->h()); // draw a white background
 copy_surf->draw(g); // draw the g widget in the clipboard
 delete copy_surf; // after this, the clipboard is loaded
 Fl_Display_Device::display_device()->set_current();  // direct graphics requests back to the display
 \endcode
 Platform details:
 \li MSWindows: Transparent RGB images copy without transparency.
 The graphical data is copied to the clipboard as an 'enhanced metafile'.
 \li Mac OS: The graphical data is copied to the clipboard (a.k.a. pasteboard) in 2 'flavors': 
 1) in vectorial form as PDF data; 2) in bitmap form as a TIFF image (or PICT for Mac OS 10.3). 
 Applications to which the clipboard content is pasted can use the flavor that suits them best.
 \li X11: the graphical data is copied to the clipboard as an image in BMP format.
*/
class FL_EXPORT Fl_Copy_Surface : public Fl_Surface_Device {
private:
  int width;
  int height;
  Fl_Paged_Device *helper;
#ifdef __APPLE__
  CFMutableDataRef pdfdata;
  CGContextRef oldgc;
  CGContextRef gc;
  void prepare_copy_pdf_and_tiff(int w, int h);
  void complete_copy_pdf_and_tiff();
  void init_PDF_context(int w, int h);
  static size_t MyPutBytes(void* info, const void* buffer, size_t count);
#elif defined(WIN32)
  HDC oldgc;
  HDC gc;
#else // Xlib
  Fl_Offscreen xid;
  Window oldwindow;
  Fl_Surface_Device *_ss;
#endif
public:
  static const char *class_id;
  const char *class_name() {return class_id;};
  Fl_Copy_Surface(int w, int h);
  ~Fl_Copy_Surface();
  void set_current();
  void draw(Fl_Widget* widget, int delta_x = 0, int delta_y = 0);
};

#if defined(__APPLE__)

/* Mac class to reimplement Fl_Paged_Device::printable_rect() */
class FL_EXPORT Fl_Quartz_Surface_ : public Fl_System_Printer {
protected:
  int width;
  int height;
public:
  static const char *class_id;
  const char *class_name() {return class_id;};
  Fl_Quartz_Surface_(int w, int h);
  virtual int printable_rect(int *w, int *h);
  virtual ~Fl_Quartz_Surface_() {};
};

#elif defined(WIN32)

/* Win class to implement translate()/untranslate() */
class FL_EXPORT Fl_GDI_Surface_ : public Fl_Paged_Device {
  int width;
  int height;
  unsigned depth;
  POINT origins[10];
public:
  static const char *class_id;
  const char *class_name() {return class_id;};
  Fl_GDI_Surface_();
  virtual void translate(int x, int y);
  virtual void untranslate();
  virtual ~Fl_GDI_Surface_();
};

#elif !defined(FL_DOXYGEN)

/* Xlib class to implement translate()/untranslate() */
class FL_EXPORT Fl_Xlib_Surface_ : public Fl_Paged_Device {
public:
  static const char *class_id;
  const char *class_name() {return class_id;};
  Fl_Xlib_Surface_();
  virtual void translate(int x, int y);
  virtual void untranslate();
  virtual ~Fl_Xlib_Surface_();
};

#endif

#endif // Fl_Copy_Surface_H

//
// End of "$Id: Fl_Copy_Surface.H 10406 2014-10-29 19:57:42Z cand $".
//
