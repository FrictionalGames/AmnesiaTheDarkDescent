//
// "$Id: fl_ask.H 9748 2012-12-11 17:06:41Z manolo $"
//
// Standard dialog header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2011 by Bill Spitzak and others.
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

/** \file fl_ask.H
 API for common dialogs.
 */

#ifndef fl_ask_H
#  define fl_ask_H

#  include "Enumerations.H"

class Fl_Widget;

/** Different system beeps available. 
 \sa fl_beep(int) */
enum Fl_Beep {
  FL_BEEP_DEFAULT = 0,  ///< Default beep.
  FL_BEEP_MESSAGE,  ///< Message beep.
  FL_BEEP_ERROR,  ///< Error beep.
  FL_BEEP_QUESTION,  ///< Question beep.
  FL_BEEP_PASSWORD,  ///< Password beep.
  FL_BEEP_NOTIFICATION  ///< Notification beep.
};

#  ifdef __GNUC__
#    define __fl_attr(x) __attribute__ (x)
#  else
#    define __fl_attr(x)
#  endif // __GNUC__

FL_EXPORT void fl_beep(int type = FL_BEEP_DEFAULT);
FL_EXPORT void fl_message(const char *,...) __fl_attr((__format__ (__printf__, 1, 2)));
FL_EXPORT void fl_alert(const char *,...) __fl_attr((__format__ (__printf__, 1, 2)));
// fl_ask() is deprecated since it uses "Yes" and "No" for the buttons,
// which does not conform to the current FLTK Human Interface Guidelines.
// Use fl_choice() instead with the appropriate verbs instead.
FL_EXPORT int fl_ask(const char *,...) __fl_attr((__format__ (__printf__, 1, 2), __deprecated__));
FL_EXPORT int fl_choice(const char *q,const char *b0,const char *b1,const char *b2,...) __fl_attr((__format__ (__printf__, 1, 5)));
FL_EXPORT const char *fl_input(const char *label, const char *deflt = 0, ...) __fl_attr((__format__ (__printf__, 1, 3)));
FL_EXPORT const char *fl_password(const char *label, const char *deflt = 0, ...) __fl_attr((__format__ (__printf__, 1, 3)));

FL_EXPORT Fl_Widget *fl_message_icon();
extern FL_EXPORT Fl_Font fl_message_font_;
extern FL_EXPORT Fl_Fontsize fl_message_size_;
inline void fl_message_font(Fl_Font f, Fl_Fontsize s) {
  fl_message_font_ = f; fl_message_size_ = s;}

FL_EXPORT void fl_message_hotspot(int enable);
FL_EXPORT int fl_message_hotspot(void);

FL_EXPORT void fl_message_title(const char *title);
FL_EXPORT void fl_message_title_default(const char *title);

// pointers you can use to change FLTK to a foreign language:
extern FL_EXPORT const char* fl_no;
extern FL_EXPORT const char* fl_yes;
extern FL_EXPORT const char* fl_ok;
extern FL_EXPORT const char* fl_cancel;
extern FL_EXPORT const char* fl_close;
#endif // !fl_ask_H

//
// End of "$Id: fl_ask.H 9748 2012-12-11 17:06:41Z manolo $".
//
