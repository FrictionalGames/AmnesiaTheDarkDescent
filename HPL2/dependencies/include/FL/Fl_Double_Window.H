//
// "$Id: Fl_Double_Window.H 9637 2012-07-24 04:37:22Z matt $"
//
// Double-buffered window header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Double_Window widget . */

#ifndef Fl_Double_Window_H
#define Fl_Double_Window_H

#include "Fl_Window.H"

/**
  The Fl_Double_Window provides a double-buffered window.
  If possible this will use the X double buffering extension (Xdbe).  If
  not, it will draw the window data into an off-screen pixmap, and then
  copy it to the on-screen window.
  <P>It is highly recommended that you put the following code before the
  first show() of <I>any</I> window in your program: </P>
  \code
  Fl::visual(FL_DOUBLE|FL_INDEX)
  \endcode
  This makes sure you can use Xdbe on servers where double buffering
  does not exist for every visual.
*/
class FL_EXPORT Fl_Double_Window : public Fl_Window {
protected:
  void flush(int eraseoverlay);
  /**
    Force double buffering, even if the OS already buffers windows
    (overlays need that on MacOS and Windows2000)
  */
  char force_doublebuffering_;
public:
  void show();
  void show(int a, char **b) {Fl_Window::show(a,b);}
  void flush();
  void resize(int,int,int,int);
  void hide();
  ~Fl_Double_Window();
  
  /**
   Creates a new Fl_Double_Window widget using the given
   position, size, and label (title) string.
  */
  Fl_Double_Window(int W, int H, const char *l = 0);
  
  /**
   See  Fl_Double_Window::Fl_Double_Window(int w, int h, const char *label = 0)
  */
  Fl_Double_Window(int X, int Y, int W, int H, const char *l = 0);

};

#endif

//
// End of "$Id: Fl_Double_Window.H 9637 2012-07-24 04:37:22Z matt $".
//
