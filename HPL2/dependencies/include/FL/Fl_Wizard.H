//
// "$Id: Fl_Wizard.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Fl_Wizard widget definitions.
//
// Copyright 1999-2010 by Easy Software Products.
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
   Fl_Wizard widget . */

//
// Include necessary header files...
//

#ifndef _Fl_Wizard_H_
#  define _Fl_Wizard_H_

#  include <FL/Fl_Group.H>


/**
    This widget is based off the Fl_Tabs
    widget, but instead of displaying tabs it only changes "tabs" under
    program control. Its primary purpose is to support "wizards" that
    step a user through configuration or troubleshooting tasks.

    <P>As with Fl_Tabs, wizard panes are composed of child (usually
    Fl_Group) widgets. Navigation buttons must be added separately.
*/
class FL_EXPORT Fl_Wizard : public Fl_Group {

  Fl_Widget *value_;

  void draw();

  public:

  Fl_Wizard(int, int, int, int, const char * = 0);

  void		next();
  void		prev();
  Fl_Widget	*value();
  void		value(Fl_Widget *);
};

#endif // !_Fl_Wizard_H_

//
// End of "$Id: Fl_Wizard.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
