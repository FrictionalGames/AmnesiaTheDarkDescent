//
// "$Id: Fl_Choice.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Choice header file for the Fast Light Tool Kit (FLTK).
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
   Fl_Choice widget . */

#ifndef Fl_Choice_H
#define Fl_Choice_H

#include "Fl_Menu_.H"

/**
  \class Fl_Choice
  \brief A button that is used to pop up a menu.

  This is a button that, when pushed, pops up a menu (or hierarchy of menus)
  defined by an array of Fl_Menu_Item objects.
  Motif calls this an OptionButton.

  The only difference between this and a Fl_Menu_Button is that the name of
  the most recent chosen menu item is displayed inside the box, while the
  label is displayed outside the box. However, since the use of this is most
  often to control a single variable rather than do individual callbacks,
  some of the Fl_Menu_Button methods are redescribed here in those terms.

  When the user picks an item off the menu the value() is set to that item
  and then the item's callback is done with the menu_button as the
  \c Fl_Widget* argument. If the item does not have a callback the
  menu_button's callback is done instead.

  All three mouse buttons pop up the menu. The Forms behavior of the first
  two buttons to increment/decrement the choice is not implemented.  This
  could be added with a subclass, however.

  The menu will also pop up in response to shortcuts indicated by putting
  a '\&' character in the label().  See Fl_Button::shortcut(int s) for a
  description of this.

  Typing the shortcut() of any of the items will do exactly the same as when
  you pick the item with the mouse.  The '\&' character in item names are
  only looked at when the menu is popped up, however.

  \image html choice.png
  \image latex choice.png  "Fl_Choice" width=4cm
  \todo Refactor the doxygen comments for Fl_Choice changed() documentation.

  \li <tt>int Fl_Widget::changed() const</tt>
      This value is true the user picks a different value. <em>It is turned
      off by value() and just before doing a callback (the callback can turn
      it back on if desired).</em>
  \li <tt>void Fl_Widget::set_changed()</tt>
      This method sets the changed() flag.
  \li <tt>void Fl_Widget::clear_changed()</tt>
      This method clears the changed() flag.
  \li <tt>Fl_Boxtype Fl_Choice::down_box() const</tt>
      Gets the current down box, which is used when the menu is popped up.
      The default down box type is \c FL_DOWN_BOX.
  \li <tt>void Fl_Choice::down_box(Fl_Boxtype b)</tt>
      Sets the current down box type to \p b.
 */
class FL_EXPORT Fl_Choice : public Fl_Menu_ {
protected:
  void draw();
public:
  int handle(int);

  Fl_Choice(int X, int Y, int W, int H, const char *L = 0);

  /**
    Gets the index of the last item chosen by the user.
    The index is zero initially.
   */
  int value() const {return Fl_Menu_::value();}

  int value(int v);

  int value(const Fl_Menu_Item* v);
};

#endif

//
// End of "$Id: Fl_Choice.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
