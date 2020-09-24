//
// "$Id: names.h 9299 2012-03-23 16:47:53Z manolo $"
//
// Event names header file for the Fast Light Tool Kit (FLTK).
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

// Thanks to Greg Ercolano for this addition.

#ifndef FL_NAMES_H
#define FL_NAMES_H

/** \defgroup fl_events Events handling functions
    @{
 */

/**
  This is an array of event names you can use to convert event numbers into names.

  The array gets defined inline wherever your '\#include <FL/names.h>' appears.

  \b Example:
  \code
  #include <FL/names.h>		// array will be defined here
  int MyClass::handle(int e) {
      printf("Event was %s (%d)\n", fl_eventnames[e], e);
      // ..resulting output might be e.g. "Event was FL_PUSH (1)"..
      [..]
  }
  \endcode
 */
const char * const fl_eventnames[] =
{
  "FL_NO_EVENT",
  "FL_PUSH",
  "FL_RELEASE",
  "FL_ENTER",
  "FL_LEAVE",
  "FL_DRAG",
  "FL_FOCUS",
  "FL_UNFOCUS",
  "FL_KEYDOWN",
  "FL_KEYUP",
  "FL_CLOSE",
  "FL_MOVE",
  "FL_SHORTCUT",
  "FL_DEACTIVATE",
  "FL_ACTIVATE",
  "FL_HIDE",
  "FL_SHOW",
  "FL_PASTE",
  "FL_SELECTIONCLEAR",
  "FL_MOUSEWHEEL",
  "FL_DND_ENTER",
  "FL_DND_DRAG",
  "FL_DND_LEAVE",
  "FL_DND_RELEASE",
  "FL_SCREEN_CONFIGURATION_CHANGED",
  "FL_FULLSCREEN"
};

/**
  This is an array of font names you can use to convert font numbers into names.

  The array gets defined inline wherever your '\#include <FL/names.h>' appears.

  \b Example:
  \code
  #include <FL/names.h>		// array will be defined here
  int MyClass::my_callback(Fl_Widget *w, void*) {
      int fnum = w->labelfont();
      // Resulting output might be e.g. "Label's font is FL_HELVETICA (0)"
      printf("Label's font is %s (%d)\n", fl_fontnames[fnum], fnum);
      // ..resulting output might be e.g. "Label's font is FL_HELVETICA (0)"..
      [..]
  }
  \endcode
 */
const char * const fl_fontnames[] =
{
  "FL_HELVETICA",
  "FL_HELVETICA_BOLD",
  "FL_HELVETICA_ITALIC",
  "FL_HELVETICA_BOLD_ITALIC",
  "FL_COURIER",
  "FL_COURIER_BOLD",
  "FL_COURIER_ITALIC",
  "FL_COURIER_BOLD_ITALIC",
  "FL_TIMES",
  "FL_TIMES_BOLD",
  "FL_TIMES_ITALIC",
  "FL_TIMES_BOLD_ITALIC",
  "FL_SYMBOL",
  "FL_SCREEN",
  "FL_SCREEN_BOLD",
  "FL_ZAPF_DINGBATS",
};

/** @} */

#endif /* FL_NAMES_H */

//
// End of "$Id: names.h 9299 2012-03-23 16:47:53Z manolo $".
//
