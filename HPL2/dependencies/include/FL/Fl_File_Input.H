//
// "$Id: Fl_File_Input.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// File_Input header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
// Original version Copyright 1998 by Curtis Edwards.
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
   Fl_File_Input widget . */

#ifndef Fl_File_Input_H
#  define Fl_File_Input_H

#  include <FL/Fl_Input.H>

/**
  \class Fl_File_Input
  \brief This widget displays a pathname in a text input field.

  A navigation bar located above the input field allows the user to
  navigate upward in the directory tree.
  You may want to handle FL_WHEN_CHANGED events for tracking text changes
  and also FL_WHEN_RELEASE for button release when changing to parent dir.
  FL_WHEN_RELEASE callback won't be called if the directory clicked
  is the same as the current one.

  <P align=CENTER> \image html Fl_File_Input.png </P> 
  \image latex Fl_File_Input.png "Fl_File_Input"  width=6cm

  \note As all Fl_Input derived objects, Fl_File_Input may call its callback
  when losing focus (see FL_UNFOCUS) to update its state like its cursor shape.
  One resulting side effect is that you should call clear_changed() early in your callback
  to avoid reentrant calls if you plan to show another window or dialog box in the callback.
*/
class FL_EXPORT Fl_File_Input : public Fl_Input {
  
  Fl_Color	errorcolor_;
  char		ok_entry_;
  uchar		down_box_;
  short		buttons_[200];
  short		pressed_;

  void		draw_buttons();
  int		handle_button(int event);
  void		update_buttons();

public:

  Fl_File_Input(int X, int Y, int W, int H, const char *L=0);

  virtual int handle(int event);

protected:
  virtual void draw();

public:
  /** Gets the box type used for the navigation bar. */
  Fl_Boxtype	down_box() const { return (Fl_Boxtype)down_box_; }
  /** Sets the box type to use for the navigation bar.  */
  void		down_box(Fl_Boxtype b) { down_box_ = b; }

  /**
    Gets the current error color.
    \todo Better docs for Fl_File_Input::errorcolor() - is it even used?
  */
  Fl_Color	errorcolor() const { return errorcolor_; }
  /** Sets the current error color to \p c */
  void		errorcolor(Fl_Color c) { errorcolor_ = c; }

  int	value(const char *str);
  int	value(const char *str, int len);

  /**
    Returns the current value, which is a pointer to an internal buffer
    and is valid only until the next event is handled.
  */
  const char	*value() { return Fl_Input_::value(); }
};

#endif // !Fl_File_Input_H


//
// End of "$Id: Fl_File_Input.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
